# PHOTON-Chat-Room (BETA)

## English

Terminal-based TCP chat room written in C++ using Boost.Asio (runs in the terminal).

### Server
- TCP server that asynchronously accepts connections.  
- Reads messages from clients.  
- Broadcasts messages to all connected clients.  

### Client
- Connects to the TCP server using IP.  
- Reads messages from the server.  
- Sends messages to the server.  
- Uses multithreading to handle reading and writing simultaneously.  

---

## Русский

Терминальная TCP чат-комната, работающая в консоли, написанная на C++ с использованием Boost.Asio.

### Сервер
- TCP сервер, который асинхронно принимает подключения.  
- Читает сообщения от клиентов.  
- Рассылает сообщения всем подключённым клиентам.  

### Клиент
- Подключается к TCP серверу, указывая IP.  
- Получает сообщения от сервера.  
- Отправляет сообщения на сервер.  
- Использует многопоточность для одновременного чтения и отправки сообщений.  
