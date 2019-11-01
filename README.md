# Ruby SRT API [WIP]

A ruby gem which you can use to build [SRT (Secure Reliable Transport)](https://github.com/Haivision/srt "Haivisionsrt") Servers and Clients.

> Secure Reliable Transport (SRT) is an open source transport technology that optimizes streaming performance across unpredictable networks, such as the Internet.
> 
> From: Haivision

Ruby SRT is a Work in Progress:

- [x] Server
  - [ ] Send Stream
  - [x] Receive Stream
  - [ ] Send File
  - [ ] Receive File
- [x] Client
  - [x] Send Stream
  - [ ] Receive Stream
  - [ ] Send File
  - [ ] Receive File
- [ ] SRT Socket


## Table Of Contents

- [Ruby SRT API [WIP]](#ruby-srt-api-wip)
  - [Table Of Contents](#table-of-contents)
  - [Installation](#installation)
    - [Dependencies](#dependencies)
    - [rbsrt gem](#rbsrt-gem)
  - [Examples](#examples)
    - [A Simple SRT Socket Server In File Mode](#a-simple-srt-socket-server-in-file-mode)
    - [A Simple Socket Client In File Mode](#a-simple-socket-client-in-file-mode)
    - [A Simple SRT Socket Server In Live Mode](#a-simple-srt-socket-server-in-live-mode)
    - [A Simple Socket Client In Live Mode](#a-simple-socket-client-in-live-mode)
    - [A Simple Recording Server](#a-simple-recording-server)
    - [A Simple ffmpeg Sending Client](#a-simple-ffmpeg-sending-client)
  - [API](#api)
    - [`SRT` Module](#srt-module)
    - [`SRT::Socket` Class](#srtsocket-class)
    - [`SRT::Server` Class](#srtserver-class)
    - [`SRT::Connection` Class](#srtconnection-class)
    - [`SRT::Client` Class](#srtclient-class)
    - [`SRT::StreamIDComponents` Class](#srtstreamidcomponents-class)
    - [`SRT::Error` Classes](#srterror-classes)
  - [License](#license)

## Installation

Currently rbsrt must be build with llvm/clang. This will change in the future.

The gem requires Ruby 2.2 or higher.


### Dependencies

You will need to install the SRT library. Visit the [SRT Github Repo](https://github.com/Haivision/srt "SRT On Github") for detailed installation instructions.

On macOs run you can run `brew install srt` to install.


### rbsrt gem

Install with rubygems:

```sh
gem install rbsrt
```


Alternativly you can install the rbsrt Ruby gem from source.

Clone the git repo:

```sh
git clone https://github.com/spllr/rbsrt.git
```

Install:

```
cd rbsrt
rake install
```


## Examples

### A Simple SRT Socket Server In File Mode

```ruby
server = SRT::Socket.new
server.transmission_mode = :file

puts "server state=#{server.state}, id=#{server.id}"

server.bind "127.0.0.1", "5556"

puts "server #{server.state}"

server.listen 3

puts "server #{server.state}"

client = server.accept

puts "server accepted:  streamid=\"#{client.streamid}\", id=#{client.id}, tsbpdmode=#{client.tsbpdmode?}"

loop do
  break unless client.connected?
  break unless data = client.recvmsg
  puts "server received: #{data}"
rescue SRT::Error::CONNLOST => e
  puts "connection lost: #{e}"
  break
rescue SRT::Error => e
  puts "server error: #{e.class}"
end

puts "client closed #{client.closed?}"

server.close

puts "server done"
```

### A Simple Socket Client In File Mode

```ruby
client = SRT::Socket.new

client.streamid = "a ruby srt socket"
client.transmission_mode = :file

client.connect "127.0.0.1", "5556"

10.times do |i|
  payload = "Hello from ruby client #{i}"
  puts "will send: \"#{payload}\""
  client.sendmsg "Hello from ruby client #{i}"
  puts "did send: \"#{payload}\""
rescue SRT::Error => e
  puts "client error: #{e}"
end

client.close
```

### A Simple SRT Socket Server In Live Mode

```ruby
server = SRT::Socket.new
server.transmission_mode = :live

puts "server state=#{server.state}, id=#{server.id}"

server.bind "127.0.0.1", "5556"

puts "server #{server.state}"

server.listen 3

puts "server #{server.state}"

client = server.accept

puts "server accepted:  streamid=\"#{client.streamid}\", id=#{client.id}, tsbpdmode=#{client.tsbpdmode?}"

loop do
  break unless client.connected?
  break unless data = client.recvmsg
  puts "server received: #{data}"
rescue SRT::Error::CONNLOST => e
  puts "connection lost: #{e}"
  break
rescue SRT::Error => e
  puts "server error: #{e.class}"
end

puts "client closed #{client.closed?}"

server.close

puts "server done"
```

### A Simple Socket Client In Live Mode

```ruby
client = SRT::Socket.new

client.streamid = "a ruby srt socket"
client.transmission_mode = :live

client.connect "127.0.0.1", "5556"

100.times do |i|
  payload = "Hello from ruby client #{i}"
  client.sendmsg "Hello from ruby client #{i}"
  sleep 1 / 30 # mimic framerate
rescue SRT::Error => e
  puts "client error: #{e}"
end

client.close
```

### A Simple Recording Server

This example shows how a naive recorder server can be build.

The server will accept incomming SRT connections and will write whatever comes in to a file on disk.

```ruby
require 'rbsrt'

server = SRT::Server.new "0.0.0.0", "5555"

server.start do |connection|

  streaminfo = SRT::StreamIDComponents.new connection.streamid

  puts "new connection: streamid=#{streaminfo.resource_name}, connections=#{connection_count}, id=#{connection.id}"

  # only handle publishing clients
  return false unless streaminfo.mode == :publish

  output_file = File.new "recording-#{Process.pid}-#{connection.id}", "w+"
  output_file.close_on_exec = true

  connection.at_data do |chunk|
    puts "received data"
    output_file.write(chunk)
  end

  connection.at_close do
    puts "closed connection"
    output_file.close
  end
end
```


### A Simple ffmpeg Sending Client

```ruby
require 'rbsrt'
require 'uri'

puts "create srt client"

client = SRT::Client.new

streaminfo = SRT::StreamIDComponents.new :mode => :publish, 
                                         :type => :stream, 
                                         :resource_name => "my_movie.ts"

client.streamid = streaminfo.to_s

at_exit { client.close }

begin
  client.connect "127.0.0.1", "5555"
rescue SRT::Error => e
  puts "A STR Error occurred. #{e.message}"
  exit 1
rescue StandardError => e
  puts "Something went wrong: #{e.message}"
  exit 1
end

# run a ffmpeg command and write stdout to the client
require 'open3'

cmd = "ffmpeg -re -stream_loop 10 -i my_movie.ts -codec copy -f mpegts -"

Open3.popen3(cmd) do |stdin, stdout, stderr, wait_thr|
  stdout.each do |buf|
    client.sendmsg buf
  end
end
```

## API

Also see the [Haivision SRT docs](https://github.com/Haivision/srt/tree/master/docs "Haivision SRT docs"), specificly the [API-functions](https://github.com/Haivision/srt/blob/master/docs/API-functions.md) and [API](https://github.com/Haivision/srt/blob/master/docs/API.md) documents, for more information about SRT, it's API and underlaying concepts.

The Ruby SRT gem comes with the following items:


|  Item | Kind | Description |
|-------|------|-------------|
|  SRT | Module  | The root module  |
|  SRT::Socket | Class  | A general purpose SRT Socket |
|  SRT::Server | Class  | A multi-client SRT Server |
|  SRT::Connection | Class  | Used by SRT::Server to represent a client |
|  SRT::Client | Class  | A SRT Client |
|  SRT::StreamIDComponents | Class  | SRT Access Control complient streamid parser and compiler  |
|  SRT::Error | Class | The base class for a number of SRT specific errors |


### `SRT` Module

The `SRT` module has the following constants:

| Name | Kind | Description |
|------|------|-------------|
| VERSION | String | Gem version string |
| SRT_VERSION | String | The version of the linked libsrt |


### `SRT::Socket` Class

The `SRT::Socket` class is a general purpose SRT socket. It can be used to create both servers and clients.

The `SRT::Socket` supports the following methods:

| Name | Kind | Description |
|------|------|-------------|
| #accept | `SRT::Socket` | Accept a new connection |
| #bind(address, port) |  | Bind the socket to an address and port |
| #broken? | Bool | True when the socket state is `:broken` |
| #close |  | Closes the socket |
| #closed? | Bool | True the when the socket state is `:closed` |
| #closing? | Bool | True the when the socket state is `:closing` |
| #connect(address, port) | | Opens a connection to a server at `srt://#{address}:#{port}` |
| #connected? | Bool | True the when the socket state is `:conneted` |
| #connecting? | Bool | True the when the socket state is `:connecting` |
| #id | Any | An identifier for the socket. This identifier will be unique for all sockets existing at any one time but might not be unique over the lifetime of a script |
| #listen(maxbacklog) | | Start listening. Must be called after `#bind` |
| #listening? | Bool | True the when the socket state is `:listening` |
| #nonexist? | Bool | True the when the socket state is `:nonexist` |
| #opened? | Bool | True the when the socket state is `:opened` |
| #rcvsyn= | Bool | Alias for `#read_sync` |
| #rcvsyn? | Bool | Alias for `#read_sync?` |
| #read | String | Alias for `#recvmsg` |
| #read_sync= | Bool | When true, set the socket to read in a non-blocking manner |
| #read_sync? | Bool | True when the socket is readable in a non-blocking manner |
| #ready? | Bool | True when the socket is ready for usage (e.g. initialized) |
| #recvmsg | String | Read data from the socket |
| #sendmsg(string) | String | Send bytes to the socket |
| #sndsyn= | Bool | Alias of `#write_sync=` |
| #sndsyn? | Bool | Alias of `#write_sync?` |
| #state | Symbol | Returns the state of the socket. Can be one of: `:broken`, `:closed`, `:closing`, `:connected`, `:connecting`, `:listening`, `:nonexist`, `:opened`, `:ready` |
| #streamid | String | The streamid of the socket if supplied |
| #streamid= | String | The streamid of the socket, must be 512 characters or less |
| #timestamp_based_packet_delivery_mode= | Bool | Indicates if the sending socket will control the timed delivery of data (e.g. video stream) |
| #timestamp_based_packet_delivery_mode? | Bool | True if the sending socket controls the timing of data delivery |
| #trans_mode= | Symbol | Alias for `#transmission_mode=` |
| #transmission_mode= | Symbol | Controls how the socket expects data transmission. Can be set to `:live` or `:file` |
| #transtype= | Symbol | Alias for `#transmission_mode=` |
| #tsbpdmode= | Bool | Alias of `#timestamp_based_packet_delivery_mode=` |
| #tsbpdmode? | Bool | Alias of `#timestamp_based_packet_delivery_mode?` |
| #write | String | Alias of `#sendmsg` |
| #write_sync= | Any | When true, set the socket to write in a non-blocking manner |
| #write_sync? | Any | True when the socket is writable in a non-blocking manner |


### `SRT::Server` Class

The `SRT::Server` class is an implementation of a multi-client SRT server. It does not inherit from `SRT::Socket` but has similar API where applicable.

The `SRT::Server` is a higher level abstraction freeing the developer of much of the boiletplate of building a server.

The `SRT::Server` will `yield` a new `SRT::Connection` instance when a new client connects.

The `SRT::Server` supports the following methods:

The constructor takes 2 aguments, an address and a port.Both must be strings.

| Name | Kind | Description |
|------|------|-------------|
| #broken? | Bool | True when the server socket state is `:broken` |
| #closed? | Bool | True the when the server socket state is `:closed` |
| #closing? | Bool | True the when the server socket state is `:closing` |
| #connected? | Bool | True the when the server socket state is `:conneted` |
| #connecting? | Bool | True the when the server socket state is `:connecting` |
| #listening? | Bool | True the when the server socket state is `:listening` |
| #nonexist? | Bool | True the when the server socket state is `:nonexist` |
| #opened? | Bool | True the when the server socket state is `:opened` |
| #rcvsyn? | Bool | Alias for `#read_sync?` |
| #read_sync? | Bool | True when the server socket is readable in a non-blocking manner |
| #ready? | Bool | True when the server socket is ready for usage (e.g. initialized) |
| #start(&blck) | Bool | Starts the servers. The block will be executed each time a new connection is accepted. Return a falsy value to reject the connection. The block will executed with the `self` set to the server |


### `SRT::Connection` Class

The `SRT::Connection` class is used by the `SRT::Server`. You should not have to instantiate instances of this class yourself.

`SRT::Connection` does not inherit from `SRT::Socket` but has similar API where applicable.

Instances of `SRT::Connection` support the following methods:


| Name | Kind | Description |
|------|------|-------------|
| #at_close(&blck) | Block | A block which will be called when the connection closed |
| #at_data(&block) | Block | A block which will be called when new data was read |
| #broken? | Bool | True when the connection socket state is `:broken` |
| #closed? | Bool | True the when the connection socket state is `:closed` |
| #closing? | Bool | True the when the connection socket state is `:closing` |
| #connected? | Bool | True the when the connection socket state is `:conneted` |
| #connecting? | Bool | True the when the connection socket state is `:connecting` |
| #id | Any | An identifier for the connection. This identifier will be unique for all sockets existing at any one time but might not be unique over the lifetime of a script |
| #listening? | Bool | True the when the connection socket state is `:listening` |
| #nonexist? | Bool | True the when the connection socket state is `:nonexist` |
| #opened? | Bool | True the when the connection socket state is `:opened` |
| #ready? | Bool | True when the connection socket is ready for usage (e.g. initialized) |
| #sendmsg(string) | String | Send bytes to the socket |
| #sndsyn= | Bool | Alias of `#write_sync=` |
| #sndsyn? | Bool | Alias of `#write_sync?` |
| #state | Symbol | Returns the state of the socket. Can be one of: `:broken`, `:closed`, `:closing`, `:connected`, `:connecting`, `:listening`, `:nonexist`, `:opened`, `:ready` |
| #streamid | String | The streamid of the socket if supplied |
| #streamid= | String | The streamid of the socket, must be 512 characters or less |
| #timestamp_based_packet_delivery_mode= | Bool | Indicates if the sending socket will control the timed delivery of data (e.g. video stream) |
| #timestamp_based_packet_delivery_mode? | Bool | True if the sending socket controls the timing of data delivery |
| #trans_mode= | Symbol | Alias for `#transmission_mode=` |
| #transmission_mode= | Symbol | Controls how the socket expects data transmission. Can be set to `:live` or `:file` |
| #transtype= | Symbol | Alias for `#transmission_mode=` |
| #tsbpdmode= | Bool | Alias of `#timestamp_based_packet_delivery_mode=` |
| #tsbpdmode? | Bool | Alias of `#timestamp_based_packet_delivery_mode?` |
| #write | String | Alias of `#sendmsg` |
| #write_sync= | Any | When true, set the socket to write in a non-blocking manner |
| #write_sync? | Any | True when the socket is writable in a non-blocking manner |


### `SRT::Client` Class

The `SRT::Client` can be used to connect to a SRT server.

Instances of `SRT::Client` support the following methods and attributes:

| Name | Kind | Description |
|------|------|-------------|
| #broken? | Bool | True when the socket state is `:broken` |
| #close |  | Closes the socket |
| #closed? | Bool | True the when the socket state is `:closed` |
| #closing? | Bool | True the when the socket state is `:closing` |
| #connect(address, port) | | Opens a connection to a server at `srt://#{address}:#{port}` |
| #connected? | Bool | True the when the socket state is `:conneted` |
| #connecting? | Bool | True the when the socket state is `:connecting` |
| #id | Any | An identifier for the socket. This identifier will be unique for all sockets existing at any one time but might not be unique over the lifetime of a script |
| #listening? | Bool | True the when the socket state is `:listening` |
| #nonexist? | Bool | True the when the socket state is `:nonexist` |
| #opened? | Bool | True the when the socket state is `:opened` |
| #rcvsyn= | Bool | Alias for `#read_sync` |
| #rcvsyn? | Bool | Alias for `#read_sync?` |
| #read | String | Alias for `#recvmsg` |
| #read_sync= | Bool | When true, set the socket to read in a non-blocking manner |
| #read_sync? | Bool | True when the socket is readable in a non-blocking manner |
| #ready? | Bool | True when the socket is ready for usage (e.g. initialized) |
| #recvmsg | String | Read data from the socket |
| #sendmsg(string) | String | Send bytes to the socket |
| #sndsyn= | Bool | Alias of `#write_sync=` |
| #sndsyn? | Bool | Alias of `#write_sync?` |
| #state | Symbol | Returns the state of the socket. Can be one of: `:broken`, `:closed`, `:closing`, `:connected`, `:connecting`, `:listening`, `:nonexist`, `:opened`, `:ready` |
| #streamid | String | The streamid of the socket if supplied |
| #streamid= | String | The streamid of the socket, must be 512 characters or less |
| #timestamp_based_packet_delivery_mode= | Bool | Indicates if the sending socket will control the timed delivery of data (e.g. video stream) |
| #timestamp_based_packet_delivery_mode? | Bool | True if the sending socket controls the timing of data delivery |
| #trans_mode= | Symbol | Alias for `#transmission_mode=` |
| #transmission_mode= | Symbol | Controls how the socket expects data transmission. Can be set to `:live` or `:file` |
| #transtype= | Symbol | Alias for `#transmission_mode=` |
| #tsbpdmode= | Bool | Alias of `#timestamp_based_packet_delivery_mode=` |
| #tsbpdmode? | Bool | Alias of `#timestamp_based_packet_delivery_mode?` |
| #write | String | Alias of `#sendmsg` |
| #write_sync= | Any | When true, set the socket to write in a non-blocking manner |
| #write_sync? | Any | True when the socket is writable in a non-blocking manner |


### `SRT::StreamIDComponents` Class

SRT provides a [Access Control Guideline](https://github.com/Haivision/srt/blob/master/docs/AccessControl.md "SRT Access Control Guidelines") allowing a more fine grained method of specifying the intent of connection. Using the "#streamid" clients and servers can pack a number of properties on the socket.

The `SRT::StreamIDComponents` class can be used to parse and compile such an extended streamid string.

Instances of `SRT::StreamIDComponents` support the following methods and attributes:

| Name | Kind | Description |
|------|------|-------------|
| #host_name | String | The hostname associated with the socket |
| #host_name= | String | Set the host associated with the socket. When the host has a pathname, and no other `#resource_name` is specified, the path will will be used as the `#resource_name`. |
| #mode | Symbol | The mode associated with the socket. Can be one of: `:request`, `:publish`, `:bidirectional` |
| #mode= | Any | Set the mode associated with the socket. Can be one of: `:request`, `:publish`, `:bidirectional` |
| #resource_name | Any | The resource name associated with the socket. e.g. "stream1", "/movies/butterfly.ts", "movies.ts?gid=1234" |
| #resource_name= | Any | Set the resource name associated with the socket. |
| #sessionid | Any | A temporary resource identifier negotiated with the server, used just for verification. Useful when using a different authentication server to negotiate access to a resource. |
| #sessionid= | Any | Set the session id |
| #to_s | Any | Compiles the Access Control string |
| #type | Any | The type associated with the socket. Can be one of `:stream`, `:file`, `:auth` |
| #type= | Any | Set the type associated with the socket. Can be one of `:stream`, `:file`, `:auth` |
| #user_name | Any | The user name associated with the socket |
| #user_name= | Any | Set the user name associated with the socket |

You can set and get any non-standard property on the `SRT::StreamIDComponents` by simply calling it's getter or setter method:

```ruby
streamid = SRT::StreamIDComponents.new

# standard

streamid.mode = :publish
streamid.type = :live
streamid.resource_name = "movie1.ts"

# non standard

streamid.my_property = "foo"
streamid.bar = "baz"

```

You can use the `SRT::StreamIDComponents` to set and retrieve Access Control properties:

```ruby
# ... on the client side ...

client = SRT::Client.new

streamid = SRT::StreamIDComponents.new
streamid.mode = :publish
streamid.type = :live
streamid.resource_name = "movie1.ts"

client.streamid = streamid.to_s

# ...


# ... on the server side ...

client = server.accept

streamid = SRT::StreamIDComponents.new client.streamid

if streamid.mode == :publish
  # read from the client
elsif streamid.mode == :request
  # write to the client
end

# ...
```

You may also pass a hash to construct the `SRT::StreamIDComponents`:

```ruby
streamid = SRT::StreamIDComponents.new :mode => :publish,
                                       :type => :live,
                                       :resource_name => "my_movie.ts",
                                       :user_name => "admin",
                                       :sessionid => "12345",
                                       :host_name => "stream.recce.nl",
                                       :some_custom_property => "somevalue"
```

### `SRT::Error` Classes

SRT specifies a number of error types which can occur. The gem defines an Error class for each one allowing the developer to `rescue` and handle specific errors.

If you want to catch _any_ SRT error, you can `rescue` `SRT::Error`, when you need more fine grained control (e.g. non block read and write no-data errors), `rescue` the appropriate error.

Each error has both a constant, and an instance variable defined with it's corresponding error code, these codes correspond with the SRT C API error codes (e.g. `SRT::Error::ASYNCRCV::Code`, `err.code`).

The gem supplies the following SRT specific errors:

| Error | Description |
|-------|-------------|
| SRT::Error | The base class of any SRT related error |
| SRT::Error::ASYNCFAIL | ... |
| SRT::Error::ASYNCRCV | A non-blocking socket did have data available. |
| SRT::Error::ASYNCSND | A non-blocking socket could not accept data. |
| SRT::Error::BOUNDSOCK | ... |
| SRT::Error::CONGEST | ... |
| SRT::Error::CONNFAIL | ... |
| SRT::Error::CONNLOST | ... |
| SRT::Error::CONNREJ | ... |
| SRT::Error::CONNSETUP | ... |
| SRT::Error::CONNSOCK | ... |
| SRT::Error::DUPLISTEN | ... |
| SRT::Error::FILE | ... |
| SRT::Error::INVALBUFFERAPI | ... |
| SRT::Error::INVALMSGAPI | ... |
| SRT::Error::INVOP | ... |
| SRT::Error::INVPARAM | ... |
| SRT::Error::INVPOLLID | ... |
| SRT::Error::INVRDOFF | ... |
| SRT::Error::INVSOCK | ... |
| SRT::Error::INVWROFF | ... |
| SRT::Error::LARGEMSG | ... |
| SRT::Error::NOBUF | ... |
| SRT::Error::NOCONN | ... |
| SRT::Error::NOLISTEN | ... |
| SRT::Error::NOSERVER | ... |
| SRT::Error::PEERERR | ... |
| SRT::Error::RDPERM | ... |
| SRT::Error::RDVNOSERV | ... |
| SRT::Error::RDVUNBOUND | ... |
| SRT::Error::RESOURCE | ... |
| SRT::Error::SECFAIL | ... |
| SRT::Error::SOCKFAIL | ... |
| SRT::Error::THREAD | ... |
| SRT::Error::TIMEOUT | ... |
| SRT::Error::UNBOUNDSOCK | ... |
| SRT::Error::UNKNOWN | ... |
| SRT::Error::WRPERM | ... |

## License

All Ruby code is released under the MIT License. All C code is released under the Mozilla Public License, v. 2.0.
