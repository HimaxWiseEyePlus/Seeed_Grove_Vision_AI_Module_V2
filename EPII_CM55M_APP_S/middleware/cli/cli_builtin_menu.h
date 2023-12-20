BEGIN_MENU (builtin)
ADD_CMD(help, cli_builtin_help, "[cmd|menu]     describe commands or menus")
ADD_CMD(ls, cli_builtin_ls, "[menu]         List contents of the menu")
ADD_CMD(cd, cli_builtin_cd, "[menu]         change to submenu or upper(..); empty menu for top")
ADD_CMD(pwd, cli_builtin_pwd, "               Show current menu path")
ADD_CMD(load, cli_builtin_load, "{script_file}  load commands from a script file")
END_MENU

