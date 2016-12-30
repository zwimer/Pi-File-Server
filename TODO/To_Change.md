### Aesthetics

- Clarify comments
- Document more about userList != file but is stored in file
- Document more restrictions on file names
	- Can't be empty

### Functional

- Need to order jobs
- Wrap mutex around user lists for safety
- Fork child, have it wait for child, then remove all
- Read currently assumes the file exists when it is called and assumes user in user list
- getWriteAccess throw error if already have
- Do something about multiple simultanious access' to the same file by the same user
	- Either restrict it, or allow it
	- Currently access is on / off
		- Meaning if two different threads get access, then one gives it up, it gives it up for both

