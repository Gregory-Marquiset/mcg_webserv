> This project has been created as part of the 42 curriculum by [cdutel](https://github.com/Charles-Louis-Dutel), [malanglo](https://github.com/mlanglois26) and [gmarquis](https://github.com/Gregory-Marquiset).

---

## Table of Contents

* [Description](#description)
* [Instructions](#instructions)
* [CI/CD & Project management](#cicd--project-management)
* [Contributors responsibilities](#contributors-responsabilities)
* [Resources](#resources)

---
<a id="description"></a>

## 🌐 Description

### Webserv — HTTP Server in C++98

**Overview:**  
Webserv is a system-level project consisting of building a fully functional HTTP server in C++98. The goal is to understand how HTTP works internally by implementing core web server mechanisms from scratch.

The server must be compatible with standard web browsers and follow HTTP protocol behavior while respecting strict constraints regarding non-blocking I/O and system calls.

**Goal:**  
Implement a resilient, non-blocking HTTP server capable of:

- Handling multiple client connections simultaneously
- Serving static files
- Processing HTTP requests (GET, POST, DELETE)
- Managing file uploads
- Executing CGI scripts
- Supporting multiple ports and configurations

**Execution format:**

```Bash
./webserv [configuration_file]
```

---

### Core Constraints

- C++98 only (`-std=c++98`)
- No external libraries
- No Boost
- No crashes under any circumstances
- Single `poll()` (or equivalent: select/kqueue/epoll) for all I/O
- Fully non-blocking architecture
- No blocking read/write on sockets
- No execve of another web server

---

### Mandatory Features

#### HTTP Handling

- GET
- POST
- DELETE
- Accurate HTTP status codes
- Default error pages
- Browser compatibility

#### Server Architecture

- Non-blocking I/O
- Single poll/select/epoll/kqueue loop
- Multiple simultaneous clients
- Proper client disconnection handling
- Stress-test resilience

#### Static Content

- Serve static websites
- Directory listing (configurable)
- Default index file
- Configurable root per route

#### File Upload

- Upload handling
- Configurable upload directory
- Max body size configuration

#### CGI Support

- Execution based on file extension
- Proper environment variable handling
- Chunked request un-chunking before CGI
- EOF handling when required

---

### Configuration File

The server must be configurable through a configuration file inspired by NGINX.

It must allow:

- Multiple interface:port definitions
- Custom error pages
- Max client body size
- Route-based configuration:
  - Allowed HTTP methods
  - Redirections
  - Root directory
  - Directory listing on/off
  - Default index file
  - Upload enable + path
  - CGI execution rules

---

<a id="instructions"></a>

## 🚀 Instructions

Build instructions, usage examples, configuration examples, and testing procedures will be documented here once implementation begins.

---

<a id="cicd--project-management"></a>

## ⚙ CI/CD & Project management

This section will document:

- Repository organization
- Branching strategy
- Testing methodology
- Stress testing approach
- Code review workflow
- AI usage policy compliance (as required by subject)

Details will be added during development.

---

<a id="contributors-responsabilities"></a>

## 👥 Contributors responsibilities

Roles and responsibilities will be documented here.

Each contributor will be able to explain:

- HTTP parsing logic
- Event loop implementation
- Poll/select/epoll usage
- Request lifecycle
- Response generation
- Configuration parsing
- CGI execution flow
- Error handling strategy
- Non-blocking architecture decisions

This section will be updated during development.

---

<a id="resources"></a>

## 📑 Resources

### Documentation / References

* [RFC 1945 – HTTP/1.0](https://www.rfc-editor.org/rfc/rfc1945)
* [RFC 2616 – HTTP/1.1](https://www.rfc-editor.org/rfc/rfc2616)
* [MDN HTTP Documentation](https://developer.mozilla.org/en-US/docs/Web/HTTP)
* [NGINX Documentation](https://nginx.org/en/docs/)
* [poll() man page](https://man7.org/linux/man-pages/man2/poll.2.html)
* [select() man page](https://man7.org/linux/man-pages/man2/select.2.html)
* [Common Gateway Interface (CGI)](https://en.wikipedia.org/wiki/Common_Gateway_Interface)

---

> This is a 42 school project. No license is provided.

---
