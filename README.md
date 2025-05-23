# Cerberus
This is the account management and moderation mod for the official server.

It supports:
* Forwarding Messages To A Discord Webhook
* Username/Password Authentication
* Basic Commands (Including Reporting)
* Custom Welcome Messages

## Configuration
All configuration files should be placed in the `cerberus` directory.

### `welcome.txt`
The contents of this file will be displayed to every player when joining.

### `accounts.txt`
This contains registered accounts.

Each account consists of two lines:
1. A Username
2. An SHA-256 Password Hash

While an administrator can create new accounts in-game,
the first administrator account will have to be created manually.

### `admins.txt`
This contains the list of administrator accounts.

### `webhook.txt`
This contains the Discord webhook.

It consists of two lines:
1. The Webhook URL
2. Who Should Be Pinged When A Player Is Reported
   * Format: `<user|role>:<id>`