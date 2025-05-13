# Git commands 
#### CGP 13/4/25

I am going to add some commands here, for future reference.

## List local branches

```
git branch
```

## Switch branches

```
git switch branch-name
```

## If the branch exists on the remote (but not yet locally):
```
git checkout -b branch-name origin/branch-name
```

## Merging from remote repository

When I wanted to pick up changes that Tobyn had added to the remote 'main' branch 
and bring them into my loca; `hm0360` branch. I asked ChatGPT for help.
The conversation should be available [here](https://chatgpt.com/share/67fae120-faf8-8005-ae07-2cc56b635244)

Summary of steps:

1. Make sure you're on the hm0360 branch
```
git checkout hm0360
```
2. Fetch latest changes from the remote
```
git fetch origin
```
3. Merge changes from origin/main into your local hm0360 branch
```
git merge origin/main
```
4. Resolve conflicts (source files)
I got a number of conflicts such as this:
```
CONFLICT (content): Merge conflict in EPII_CM55M_APP_S/app/ww_projects/ww130_cli/ww130_cli.c
```
Deal with this by editing the file. Search for `<<<<<<< HEAD` then `=========` then `>>>>>>> origin/main`
The local code is above the seperator and the remote code is below.
When the conflict is resolved, do this:
```
git add EPII_CM55M_APP_S/app/ww_projects/ww130_cli/ww130_cli.c
```
5. Resolve conflicts (binary files)
I got a number of errors like this:
```
warning: Cannot merge binary files: EPII_CM55M_APP_S/prebuilt_libs/gnu/lib_cmsis_nn_7_0_0.a (HEAD vs. origin/main)
Auto-merging EPII_CM55M_APP_S/prebuilt_libs/gnu/lib_cmsis_nn_7_0_0.a
CONFLICT (content): Merge conflict in EPII_CM55M_APP_S/prebuilt_libs/gnu/lib_cmsis_nn_7_0_0.a
```
Manually choose which version to keep:
```
# To keep your local version (the one in hm0360):
git checkout --ours EPII_CM55M_APP_S/prebuilt_libs/gnu/lib_cmsis_nn_7_0_0.a

# OR to use the version from origin/main:
git checkout --theirs EPII_CM55M_APP_S/prebuilt_libs/gnu/lib_cmsis_nn_7_0_0.a
```
Then mark it as resolved:
```
git add EPII_CM55M_APP_S/prebuilt_libs/gnu/lib_cmsis_nn_7_0_0.a
```
and comit when all done:
```
git commit -m "message here"
```

## Listing Commits, and making barnches from commits

I wanted to go into the past to compare (working) old build with (broken) new build. Here are some commands:

To list commits between dates:
```
git log --since="2024-01-01" --until="2025-03-01" --pretty=format:"%h %ad %s" --date=short
```
This gives entries like this:
```
d3da3ea 2025-02-25 Revised bootloader (console output). Revised acutetech_utils (uses nRF52832 timers). 
b916d38 2025-02-15 Add WildlifeWatcher_1 and dfu)extension_test projects, other projects, more documentation,
```
I can select one or more of the commits and make branches from them, then access the branches. Like this:
```
git checkout -b feb15-version b916d38
```

To delete branches when finished with them:
```
git branch -d <branch-name>
```
Use -D (uppercase) to force-delete if Git complains



 



