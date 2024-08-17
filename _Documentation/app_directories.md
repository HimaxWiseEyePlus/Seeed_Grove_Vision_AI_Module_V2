## Changing app directories - CGP 10/8/24

I have been a bit concerned about mixing our Wildlife Watcher projects with the 
examples from Himax in the app/scenario_app folder.

Here is a quick and easy way to put our  own apps in a different folder. 
It is not a perfect solution, but it is a start

1	Create a new folder for our projects in apps:
---------------------------------------------------
Alongside 'app/scenario_app' there is now 'app/ww_projects' as possible values for PROJECT_FOLDER

2 Define PROJECT_FOLDER with these lines at the top of makefile:
------------------------------------------------------------------
I have made a chnage to makefile and the user should check that the desired PROJECT_FOLDER is selected.
```
# CGP added this to select folder containing projects
# Uncomment one of the following lines to define the project folder. There must be a corresponding entry in app.mk
PROJECT_FOLDER = scenario_app
#PROJECT_FOLDER = ww_projects

$(info Choosing PROJECT_FOLDER='${PROJECT_FOLDER}' for our projects )
```
3	Create ww.mk
------------------
I have copied 'scenario_app/scenario_app.mk' to 'app/ww_projects/ww.mk'
I edited ww.mk to point to the new project directory.
It is probably easier also to define the application name here as well (instead of in makefile): 
```
SCENARIO_APP_ROOT = $(EPII_ROOT)/app/ww_projects
APP_TYPE = i2c_slave_app
```

(It is probably possible to edit this file to change refereences to 'SCENARIO_APP_xxx' to 'WW_PROJECTS_xxx'
but I have not tested this, and it is not necessary).

4 Edit app/app.mk to include the right .mk file:
--------------------------------------------
I have made this change: 
```
# CGP choose where to look for the apps!
ifeq (${PROJECT_FOLDER}, scenario_app)
include $(EPII_ROOT)/app/scenario_app/scenario_app.mk
else
include $(EPII_ROOT)/app/ww_projects/ww.mk
endif
```

5	Checking by placing a project(s) in the new folder
-------------------------------------------------------
Initially I made a copy of i2c_slave_app in the new ww_projects folder and changed the signon message to prove 
the compiler was picking up the right file.

6	HOWTO Clone a Project
--------------------------
It is probably easiest to take a similar project directory and copy it - for example
copy ww_projects/ww_template to ww/Projects/my_new_project

Then using Eclipse Project Explorer pane, rename at least some of the files:
- ww_template.c to my_new_project.c
- ww_template.h to my_new_project.
- ww_template.ld to my_new_project.ld
- ww_template.sct to my_new_project.sct
- ww_template.mk to my_new_project.mk
 