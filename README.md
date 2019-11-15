The purpose of this repository is to create a minimal, reproducible example
of a flaw in libuwsc.

It opens a connection to  wss://echo.websocket.org and sends packets of
arbitrary text data and compares the response string. If it is different, then
it can be assumed that libuwsc has scrambled the data.

# Building

```bash
git submodule init --update
make vendor
make
```

# Running

```bash
./bin/test
```