Modules
=======

A module is a part of the bundle that will be included in the runtime only if certain restrictions are meet. The 
restrictions are set by the module authors.

-----------
The Problem
-----------

Bundled applications require certain libraries from the host system such as libstdc++. In order to ensure a given
application work on a system with an older version the required version must be bundled and used at runtime. In case the
system ships a newer version of the library the system one must be used and the shipped one ignored.

-----------------
Proposed Solution
-----------------

AppDirs may contain given set of modules at `$APPDIR/opt/`. Each module will contain the following components:

- binaries to be optionally used at runtime
- "check" app to tells whether the module should be used or not
- "config" file with the module settings

config file
-----------

The config file will follow the [libconfig format](https://hyperrealm.github.io/libconfig/libconfig_manual.html) and
will contain the settings required to activate the bundle at runtime. It may also include other information required by
other components such as the check binary. It will contain a mandatory configuration group name `module` there will be
defined the additional `library_path`, `path_mappings` and other `environment` variables that could be required.

Example:

```shell
version = "1.0";

module:
{
  # library paths to be set
  library_path = ( "${APPDIR}/opt/module_id/lib", "${APPDIR}/opt/module_id/usr/lib");
  
  # path_mappings to be set
  path_mappings= ( ( "/bin/app", "${APPDIR}/opt/module_id/bin/app") );

  # additional environment variables to be set 
  environment:
  {
    MY_VAR: "MY_VAR_VALUE";
  };
};
```