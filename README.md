# AppImage Execution Wrapper

Shared library to be preloaded with on AppImage executions. It intercepts
`execve` calls to clean up the environment if the target binary resides 
outside of the AppDir bundle.

