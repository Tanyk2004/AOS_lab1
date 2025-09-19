# set vars
USER=tanay24
GROUP=$(id -gn "$USER")

# 1) give ownership of your home back to you
sudo chown -R "$USER:$GROUP" "/users/$USER"

# 2) fix strict SSH perms
sudo chown -R "$USER:$GROUP" "/users/$USER/.ssh"
sudo chmod 700 "/users/$USER/.ssh"
sudo chmod 600 "/users/$USER/.ssh/authorized_keys" 2>/dev/null || true
sudo chmod 600 "/users/$USER/.ssh/id_"* 2>/dev/null || true
sudo chmod 644 "/users/$USER/.ssh/"*.pub 2>/dev/null || true

# 3) ensure home isn’t group/other-writable (some sshd configs require this)
sudo chmod 755 "/users/$USER"
sudo chmod -R go-w "/users/$USER"

# (optional) clean VS Code server so it reinstalls cleanly next login
sudo rm -rf "/users/$USER/.vscode-server"
