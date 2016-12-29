# Old:
```
   FH
---------       ----------------
|   ----|---->  |     name     |
---------       |   std::set   |
|       |       |   mutex 1    |
---------       |   mutex 2    |
|       |       ----------------
---------
|       |
---------
```

# New:

Requires < 1/2 GB shared mem at worst

### Files:
- Max of 2^24 files
- Named file: UserList (not a safe file!)
	- Only protection: a mutex for writing!
	- Reading only access is not allowed!
- Two named files: "FileList" and "LogFile", both of which must have this protection!
- User created files allowed
- FileList file must have
	- Other files each on their own line
	- This file maps an index, line number, to the filename.
	- Whenever a file is added to this list:
		- Everything in Section 2 must be initalized, and set up with the writer as the creator	
		- Only after this can ownership of this file be given up!
	- Each file must always end with a '\n'
- UserList
	- Line 1 = "MasterProc"
	- Other users each on their own line
	- This file maps an index, line number, to the username.
- File names cannot contain following characters
	- MaxLength (TBD, need to find out max length of named shared mem)
	- No file with the same name as a named file 
	- Anything disallowed by unix
	- '\n'
	- 0

### Users:
- Max of users: 2^4
- Can be named anything except "MasterProc"

### For each file in FileList, and for FileList and UserList: (Section 2)

1. Inter-proc-set<int> named: "File: %d Users", index
	- List of user indicies
2. Inter-proc mutex "File: %d writing", index
3. Inter-proc mutex "File: %d whoM", index (depends if functions are thread-safe)

### General
Master proc, on exit or crash, make handler that deletes EVERYTHING

- Run this on startup

