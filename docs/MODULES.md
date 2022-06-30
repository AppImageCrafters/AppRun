Modules
=======

A module is a part of the bundle that will only be used at runtime if the host system doesn't provide a certain feature.
Each module will include a "check" app (or script). This will be executed by the AppRun to verify if the system has the 
required feature. The module will be used only if this check fails.

Modules provide a way of configuring the bundle at runtime. This is required to switch between the system and bundled 
libraries depending on the system configuration. The simplest example is glibc, applications need to always use the
latest version therefore we must check if the system provides an older one in order to use the module.

-------------
Specification
-------------

An AppDir may contain given set of modules at `$APPDIR/opt/`. Each module will contain the following components:

- "check" app
- "config" file
- binaries and resources to be optionally used at runtime

check
-----
The check application has the sole purpose of guessing whether the system provides certain feature. It will be executed
on launch and must return a fast a possible. The application must exit with code 0 if the system has the expected
feature otherwise must return a non-zero value.

The module will be enabled only if the system doesn't have the required feature (which means that the check returned a 
non-zero value).

config file
-----------

The config file will follow the [libconfig format](https://hyperrealm.github.io/libconfig/libconfig_manual.html) and
will contain the settings required to activate the bundle at runtime. It may also include other information required by
other components such as the check binary. It will have the following structure:

- `check`: optional group, may contain data required by the check app.
- `module`: mandatory group, module runtime settings
  
  - `library_paths`: optional string list, module library paths
  - `path_mappings`: optional string pair list, path mapping instructions
  - `environment`: optional map, environment variables
  - `runtime_dir`: optional string, working directory to be set on execve instructions (reserved for the glibc module)

Example:

```shell
version = "1.0";

# check app configuration
check:
{
  required_lib_version = "2.0.0";
};

# module runtime configuration
module:
{
  # library paths to be set
  library_paths = ( "${APPDIR}/opt/module_id/lib", "${APPDIR}/opt/module_id/usr/lib");
  
  # path_mappings to be set
  path_mappings = [ "/bin/app:${APPDIR}/opt/module_id/bin/app" ] ;

  # additional environment variables to be set 
  environment:
  {
    MY_VAR: "MY_VAR_VALUE";
  };
};
```
