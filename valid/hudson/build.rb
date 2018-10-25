#!/usr/bin/ruby

$LOAD_PATH.push('metabuild/lib')
require 'metabuild'
include Metabuild

options = Options.new({ "target"              => "k1",
			"version"             => ["1-0", "Tools version."],
			"march"               => ["k1c", "List of mppa_architectures."],
			"march_valid"         => ["k1c", "List of mppa_architectures to validated on execution_platform."],
			"platform"            => ["elf,rtems,nodeos", "List of targeted platforms"],
			"execution_platform"  => {
			  "type"     => "keywords",
			  "keywords" => [:hw, :sim],
			  "default"  => "sim",
			  "help"     => "Execution platform: can be hardware (jtag, PCI) or simulation (k1-mppa, k1-cluster)."
			},
			"board"               => {
			  "type"     => "keywords",
			  "keywords" => [:csp_generic],
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
    `PATH=#{toolroot}/bin:$PATH k1-#{os_flav}-gcc -print-multi-lib `.each_line do |multilib_line|
      multi_dir = multilib_line.split(';')[0].chomp()
      multi_opts = multilib_line.split(';')[1].gsub('@', ' -').chomp()
      cmd = "echo __k1arch | PATH=#{toolroot}/bin:$PATH k1-#{os_flav}-gcc  #{multi_opts} -  -E -P | grep -v define"
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
raise "Only k1c architecture supported for now, update required!" unless march_hash.has_key? "k1c"

march = march_hash.keys[0]

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
install = ParallelTarget.new("install", repo, [build], [])
package = Target.new("package", repo, [install], [])

valid   = ParallelTarget.new("valid", repo, [], [])

install.write_prefix()

b = Builder.new("mppadl", options, [clean, doc, build, valid, install, package])

b.default_targets = [doc, package]
distrib_info      = b.get_distrib_info()

b.logsession = arch

if toolchain == "bare" then
  os_flavors = ["mbr"]
else
  raise "Building for #{toolchain} toolchain not possible, only bare supported"
end

glob_banned_multilib = [ ]

autotraces_enable = true

build_dirs      = []
flag_options    = []
lib_options     = []
native_builds   = []
cmake_opts      = []
build_types     = []
machine_types   = []


make_env = {
    "K1_TOOLCHAIN_DIR" => toolroot,
    "PATH" => "#{install_prefix}/bin:#{toolroot}/bin:#{ENV.fetch("PATH","")}",
    "DOXYGEN_DIR" => doxygen_dir,
    "ARCH" => march,
}

b.target("doc") do
  b.logtitle = "Report for mppadl doc"

  mkdir_p root_build_dir

  cd mppadl_path

  b.run(:cmd => "make doc",
        :env => make_env.merge({"O"=>root_build_dir}))
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
	  extra_flags = "-g3 -O0"
	end

	cflags = " #{multi_opts} #{extra_flags}"
        local_env = make_env.merge(
            {
                "O" => build_dir,
                "CFLAGS" => "\"#{cflags}\"",
                "LDFLAGS" => "\"#{cflags}\"",
            })

	b.run(:cmd => "make",
              :env => local_env)
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

	    b.cd! File.join(mppadl_path, "tests")
	    flag_opt = " #{multi_opts}"

	    make_defvar = "K1_TOOLCHAIN_DIR='#{toolroot}' " +
	                  "SPEC_CFLAGS='#{multi_opts} ' " +
	                  "SPEC_LDFLAGS='#{multi_opts}' " +
	                  "OS=#{os_flav} " +
	                  "DESTDIR='#{tests_build_dir}' " +
	                  "MCORE=#{multi_arch} " +
	                  "EXECUTION_PLATFORM=#{execution_platform} " +
	                  "BOARD=#{board}"

	    b.valid(:cmd => "make #{make_defvar} all",
		    :name => "BUILD TESTS for #{os_flav} / #{multi_opts}",
                    :env => make_env)
	    b.valid(:cmd => "make #{make_defvar} #{valid_jobs} all-tests",
		    :name => "RUN TESTS for #{os_flav} / #{multi_opts}",
                    :env => make_env)
        end
    end
    b.valid(:cmd => "cd #{File.join(root_build_dir, 'tests')}; " +
                    "#{File.join(mppadl_path, 'tests', 'build_summary.sh')}",
	    :name => "BUILD SUMMARY OF RELOCS/PSEUDO_FUNC USED (should never fail)",
            :env => make_env)

    b.valid(:cmd => "#{File.join(mppadl_path, 'tests', 'gen_test_report.sh')} #{File.join(root_build_dir, 'tests')}",
	    :name => "Tests SUMMARY (read this in case of failure!)")
end

b.target("install") do
  b.logtitle = "Report for mppadl install, arch = #{arch}"

  case arch
  when "k1"
    multilibs = get_multilibs(toolroot, os_flavors, cores_list, glob_banned_multilib)
    multilibs.each do |os_flav, os_multilibs|
      os_multilibs.each do |multi_dir, multi_opts, multi_arch, multi_id|

	if os_flav == "mbr"
	  libdir     = File.join(install_prefix, "#{arch}-mbr", "lib", multi_dir)
	  includedir = File.join(install_prefix, "#{arch}-mbr", "include")
	else
            raise "unreachable: unsupported OS #{os_flav}"
	end

	build_dir = File.join(root_build_dir, build_type, os_flav, multi_arch, multi_id)

	mkdir_p build_dir
	mkdir_p libdir
	mkdir_p includedir
	mkdir_p "#{includedir}/arch"

	cd mppadl_path

	if multi_arch.include? "io"
	  cluster_target = "io"
	else
	  cluster_target = "cluster"
	end

        install_make_env = make_env.merge(
            {
                "O" => build_dir,
                "INSTALL_LIBDIR" => libdir,
                "INSTALL_INCLUDEDIR" => includedir,
                "DOC_PREFIX" => kalray_internal,
            })

	b.run(:cmd => "make install",
              :env => install_make_env)

	# Copy to toolroot
	b.rsync(install_prefix,toolroot)
      end
    end
  end

  b.run(:cmd => "make doc-install",
        :env => make_env.merge(
            {
                "O"=>root_build_dir,
                "DOC_PREFIX" => kalray_internal,
            }))
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
