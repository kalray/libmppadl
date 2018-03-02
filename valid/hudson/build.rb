#!/usr/bin/ruby

$LOAD_PATH.push('metabuild/lib')
require 'metabuild'
include Metabuild

options = Options.new({ "target"              => "k1",
			"version"             => ["1-0", "Tools version."],
			"march"               => ["k1b:k1bdp,k1bio", "List of mppa_architectures."],
			"march_valid"         => ["k1b:k1bdp,k1bio", "List of mppa_architectures to validated on execution_platform."],
			"processor"           => "processor",
			"platform"            => ["elf,rtems,nodeos", "List of targeted platforms"],
			"execution_platform"  => {
			  "type"     => "keywords",
			  "keywords" => [:hw, :sim],
			  "default"  => "sim",
			  "help"     => "Execution platform: can be hardware (jtag, PCI) or simulation (k1-mppa, k1-cluster)."
			},
			"board"               => {
			  "type"     => "keywords",
			  "keywords" => [:developer, :explorer, :pcie_530, :emb01, :tc2, :tc3, :konic80, :ab04, :konic40],
			  "default"  => "developer",
			  "help"     => "Target board (changing things at compilation)."
			},
			"toolchain"           => {
			  "type"     => "keywords",
			  "keywords" => [:default, :bare, :rtems, :linux],
			  "default"  => "default",
			  "help"     => "Enable to build only utils i.e. for bare toolchain"
			},
			"build_type"          => {
			  "type"     => "keywords",
			  "keywords" => [:Debug, :Release],
			  "default"  => "Debug",
			  "help"     => "Build type."
			},
		      })


def get_multilibs(toolroot, os_flavors, cores, banned_opts)
  multilibs = {}

  os_flavors.each do |os_flav|
    multilibs[os_flav] = []
  end

  os_flavors.each do |os_flav|
    `PATH=#{toolroot}/bin:$PATH k1-gcc -print-multi-lib -mos=#{os_flav}`.each_line do |multilib_line|
      multi_dir = multilib_line.split(';')[0].chomp()
      multi_opts = multilib_line.split(';')[1].gsub('@', ' -').chomp()
      cmd = "echo __k1arch | PATH=#{toolroot}/bin:$PATH k1-gcc -mos=#{os_flav} #{multi_opts}  -  -E -P | grep -v define"
      multi_arch = `#{cmd}`.chomp()
      multi_id = multilib_line.split(';')[1].gsub('=', '@').chomp()
      skip_opt = false

      banned_opts.each do |banned_opt|
	if multi_opts.include? banned_opt
	  skip_opt = true
	end
      end

      if ! (cores.include? multi_arch) then
	skip_opt = true
      end

      if not skip_opt
	multilibs[os_flav] += [[multi_dir, multi_opts, multi_arch,multi_id]]
      end
    end
  end
  return multilibs
end


arch         = options["target"]
workspace    = options["workspace"]
mppadl_clone = options["clone"]
build_type   = options["build_type"]
jobs         = options['jobs']


mppadl_path     = File.join(workspace, mppadl_clone)
prefix          = options.fetch("prefix", File.expand_path("none",workspace))
install_prefix  = File.join(prefix,"mppadl","devimage")
toolroot        = options.fetch("toolroot", File.expand_path("none",workspace))
pkg_prefix_name = options.fetch("pi-prefix-name","#{arch}-")
kalray_internal = File.join(prefix,"kalray_internal")


march_valid_hash = Hash[*options["march_valid"].split(/::/).map{|tmp_arch| tmp_arch.split(/:/)}.flatten]
march_hash       = Hash[*options["march"].split(/::/).map{|tmp_arch| tmp_arch.split(/:/)}.flatten]

raise "Multiple k1 architectures is not supported for now, update required!" if march_hash.keys.size != 1
raise "Only k1b architecture supported for now, update required!" unless march_hash.has_key? "k1b"

cores_list = []
march_hash.each{|k,v| cores_list.push v.split(/,/)}
cores_list = cores_list.flatten

cores_valid_list = []
march_valid_hash.each{|k,v| cores_valid_list.push v.split(/,/)}
cores_valid_list = cores_valid_list.flatten

execution_platform = options['execution_platform'].to_s
board              = options['board'].to_s

root_build_dir        = File.join(mppadl_path, "build")
root_tmp_install_dir  = File.join(root_build_dir, "tmp_install")
root_tmp_multilib_dir = File.join(root_tmp_install_dir, install_prefix)
doxygen_dir           = File.join(workspace, "kEnv/k1tools/usr/local/k1Req", "doxygen", "1.8.8")

target     = options["target"]
toolchain  = options["toolchain"].to_s
build_type = options['build_type'].to_s

repo = Git.new(mppadl_clone,workspace)

clean   = CleanTarget.new("clean", repo, [])
build   = ParallelTarget.new("build", repo, [], [])
doc     = Target.new("doc", repo, [])
valid   = ParallelTarget.new("valid", repo, [build], [])
install = ParallelTarget.new("install", repo, [valid], [])
package = Target.new("package", repo, [install], [])

install.write_prefix()

b = Builder.new("mppadl", options, [clean, doc, build, valid, install, package])

b.default_targets = [doc, package]
distrib_info      = b.get_distrib_info()

b.logsession = arch

if toolchain == "bare" then
  os_flavors = ["bare"]
else
  os_flavors = ["bare",	"nodeos","rtems"]
end

glob_banned_multilib = ["-m64"]

autotraces_enable = true

build_dirs      = []
flag_options    = []
lib_options     = []
native_builds   = []
cmake_opts      = []
build_types     = []
machine_types   = []


b.target("doc") do
  b.logtitle = "Report for mppadl doc"

  mkdir_p root_build_dir

  cd mppadl_path

  b.run("K1_TOOLCHAIN_DIR='#{toolroot}' O=#{root_build_dir} DOXYGEN_DIR='#{doxygen_dir}' make doc")
end


b.target("build") do
  b.logtitle = "Report for mppadl build"

  case arch
  when "k1"
    multilibs = get_multilibs(toolroot, os_flavors, cores_list, glob_banned_multilib)
    multilibs.each do |os_flav, os_multilibs|
      os_multilibs.each do |multi_dir, multi_opts, multi_arch, multi_id|

	build_dir = File.join(root_build_dir, build_type, os_flav, multi_arch, multi_id)

	mkdir_p build_dir

	cd mppadl_path

	extra_flags = ""
	if build_type == "Debug" then
	  extra_flags = "-g"
	end

	if multi_arch.include? "io"
	  cluster_target = "io"
	else
	  cluster_target = "cluster"
	end

	cflags = "-mos=#{os_flav} #{multi_opts} #{extra_flags}"

	b.run("K1_TOOLCHAIN_DIR='#{toolroot}' O=#{build_dir} CLUSTER_TARGET=#{cluster_target} CFLAGS='#{cflags}' make")

      end
    end
  end
end


b.target("clean") do
  b.logtitle = "Report for mppadl clean, arch = #{arch}"

  b.run("rm -rf #{root_build_dir}")
end


b.target("valid") do
  b.logtitle = "Report for mppadl valid, arch = #{arch}"

  case arch
    when "k1"
      if execution_platform == "sim" then
	valid_jobs= "-j #{jobs}"
      else
	valid_jobs="-j1"
      end

    # skip fPIC and fno-exception as they make no sense in tests.
    skip_multilib = ["-fPIC", "-fno-exceptions"]
    ## JTAG not yet ready for 64bits executables
    ## GCC seems to badly generate 64bits code (uninitialized stack)
    skip_multilib.push "-m64"

    multilibs = get_multilibs(toolroot, os_flavors, cores_valid_list, skip_multilib)

    multilibs.each do |os_flav, os_multilibs|
      os_multilibs.each do |multi_dir, multi_opts, multi_arch, multi_id|

	lib_build_dir   = File.join(root_build_dir, build_type, os_flav, multi_arch, multi_id)
	tests_build_dir = File.join(root_build_dir, "tests", build_type, os_flav, multi_arch, multi_id)

	b.create_goto_dir! tests_build_dir

	arch="k1b"

	b.cd! File.join(mppadl_path, "tests")
	case multi_arch
	when "k1dp"
	  next # skip Andey
	when "k1io"
	  next # skip Andey
	when "k1bio"
	  cluster_type = "ioddr"
	when "k1bdp"
	  cluster_type = "node"
	end
	flag_opt = " #{multi_opts}"

	if multi_arch.include? "io"
	  cluster_target = "io"
	else
	  cluster_target = "cluster"
	end

	make_defvar = "K1_TOOLCHAIN_DIR='#{toolroot}' " +
	  "SPEC_CFLAGS='#{multi_opts} -mos=#{os_flav}' " +
	  "SPEC_LDFLAGS='#{multi_opts} -mos=#{os_flav}' " +
	  "OS=#{os_flav} " +
	  "DESTDIR='#{tests_build_dir}' " +
	  "MPPADL_LIB_PATH=#{lib_build_dir}/lib/#{cluster_target} " +
	  "CLUSTER_TYPE=#{cluster_type} " +
	  "ARCH=#{arch} " +
	  "MCORE=#{multi_arch} " +
	  "EXECUTION_PLATFORM=#{execution_platform} " +
	  "BOARD=#{board}"

	b.valid(:cmd => "K1_TOOLCHAIN_DIR='#{toolroot}' PATH=#{install_prefix}/bin:#{toolroot}/bin:$PATH make #{make_defvar} all",
		:name => "BUILD TESTS for #{os_flav} / #{multi_opts}")
	b.valid(:cmd => "K1_TOOLCHAIN_DIR='#{toolroot}' PATH=#{install_prefix}/bin:#{toolroot}/bin:$PATH make #{make_defvar} #{valid_jobs} all-tests",
		:name => "RUN TESTS for #{os_flav} / #{multi_opts}")
      end
    end

    b.valid(:cmd => "#{File.join(mppadl_path, 'tests', 'gen_test_report.sh')} #{File.join(root_build_dir, 'tests')}",
	    :name => "Tests SUMMARY (read this in case of failure!)")
  end
end

b.target("install") do
  b.logtitle = "Report for mppadl install, arch = #{arch}"

  case arch
  when "k1"
    multilibs = get_multilibs(toolroot, os_flavors, cores_list, glob_banned_multilib)
    multilibs.each do |os_flav, os_multilibs|
      os_multilibs.each do |multi_dir, multi_opts, multi_arch, multi_id|

	if os_flav == "bare"
	  libdir     = File.join(install_prefix, "#{arch}-elf", "lib", multi_dir)
	  includedir = File.join(install_prefix, "#{arch}-elf", "include")
	elsif os_flav == "nodeos"
	  libdir     = File.join(install_prefix, "#{arch}-nodeos", "lib", multi_dir)
	  includedir = File.join(install_prefix, "#{arch}-nodeos", "include")
	elsif os_flav == "rtems"
	  libdir     = File.join(install_prefix, "#{arch}-rtems", "lib", multi_dir)
	  includedir = File.join(install_prefix, "#{arch}-rtems", "include")
	end

	build_dir = File.join(root_build_dir, build_type, os_flav, multi_arch, multi_id)

	mkdir_p build_dir
	mkdir_p libdir
	mkdir_p includedir

	cd mppadl_path

	if multi_arch.include? "io"
	  cluster_target = "io"
	else
	  cluster_target = "cluster"
	end

	b.run("K1_TOOLCHAIN_DIR='#{toolroot}' O=#{build_dir} CLUSTER_TARGET=#{cluster_target} " +
	      "INSTALL_LIBDIR=#{libdir} INSTALL_INCLUDEDIR=#{includedir} DOC_PREFIX=#{kalray_internal} make install")

	# Copy to toolroot
	b.rsync(install_prefix,toolroot)
      end
    end
  end

  b.run("K1_TOOLCHAIN_DIR='#{toolroot}' O=#{root_build_dir} DOC_PREFIX=#{kalray_internal} make doc-install")
end


b.target("package") do
  b.logtitle = "Report for mppadl packaging, arch = #{arch}, type = #{build_type}"

  if (distrib_info.name != "MinGW")
    cd install_prefix

    mppadl_name = "#{pkg_prefix_name}libmppadl"
    mppadl_tar  = "#{mppadl_name}.tar"
    b.run("tar cf #{mppadl_tar} ./*")
    tar_package = File.expand_path(mppadl_tar)

    depends = []

    package_description = "#{arch.upcase} mppadl (dynamic loader) host runtime package.\n"
    package_description += "This package provides dynamic loading for #{arch.upcase}."

    (version,buildID) = options["version"].split("-")
    release_info = b.release_info(version,buildID)
    pinfo = b.package_info(mppadl_name, release_info, package_description, depends)

    b.create_package(tar_package, pinfo)
    b.run("rm -f #{tar_package}")

  end
end

b.launch
