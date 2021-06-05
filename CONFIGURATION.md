# Configuration instructions ⚙️

The `irisha.conf` file is used for server-settings. Settings can contain spaces and tabs,
but setting line WITH comment can contain only 500 symbols!

Settings format: `<NAME> = <PARAMETER>`

Comments format: `# Comment` and `; Comment`

Main settings
-----
`server-domain`      # Server domain

`server-password`    # Server password, currently unused (it is set at startup)
 
`welcome-message`    # The message that is sent to the user when connected

`oper-password`      # Password for IRC-operator

Timeouts
-----
All timeout settings can be between 1 and 10000, ping and register timeouts
must be lower than connection timeout or setting would be default.

`ping-timeout`       # Timeout for connections pinging (in seconds)

`register-timeout`   # Time for client or server registration (in seconds)

`connection-timeout` # Time until disconnection since last message (in seconds)

Admin information
-----
This section is used mainly by ADMIN command

`admin-location`     # Admin country, city or similar information

`admin-info`         # Other admin information

`admin-mail`         # Admin email

Log messages
-----
`time-stamps`        # Time stamps (enabled if "yes", disabled if "no")
