package main

import (
	"encoding/json"
	"fmt"
	"io"
	"net"
	"os"
	"strconv"
	"strings"
	"sync"
	"time"
)

// TODO: update to be same as client
const (
	Hello = iota + 1 // 1
	Message          // 2
	Goodbye          // 3
	CurrentUsers     // 4
)

// for keeping track of connections
type ConnectionInfo struct {
	connection net.Conn
	id         int
	username   string
}

type ConnectionList struct {
	clients []ConnectionInfo
	mu      sync.Mutex
}

// using JSON for our messages
// each message should contain the message
// and an int (see the const above) representing the type of message
type MessageStruct struct {
	MsgType    string
	Msg        string
	Timestamp  string
	Sender     string
}

func dissectMessage(msg string) []string {
	return strings.Split(msg, ":")
}

func sendMessage(msg string, msgType string, conn net.Conn) {

	timestamp := strconv.FormatInt(time.Now().Unix(), 10)
	
	msgStruct := MessageStruct{
		MsgType: msgType,
		Msg: msg,
		Timestamp: timestamp,
		Sender: "the_server",
	}
	
	msgContent, _ := json.Marshal(msgStruct)

	msgLength := len(msgContent)
	
	// prepend msg with the length of msg so receiver knows how many bytes to read
	//conn.Write([]byte(string(msgLength) + msg))
	
	conn.Write([]byte(string(msgLength)))
	conn.Write(msgContent)
}

// https://opensource.com/article/18/5/building-concurrent-tcp-server-go
func handleConnection(conn net.Conn, clientId int, clientList *ConnectionList) {

	//fmt.Printf("Serving: %s\n", conn.RemoteAddr().String())
	fmt.Printf("got a client!\n");
	
	for {
		// this is to get the message length (which should be one byte)
		// we expect that the first byte read from a socket should be the length of the content
		// to expect, which we then read in completely so that any bytes after that should follow the same cycle
		buf := make([]byte, 1)
		
		// read one byte first to know how many bytes will make up the actual message
		_, err := io.ReadFull(conn, buf)
		if err != nil {
			//fmt.Println("error reading initial byte from socket")
			continue
		}
		
		// make a new buffer for the message
		numMessageBytes := int(buf[0])
		
		fmt.Printf("need to read: %d bytes for the message.\n", numMessageBytes)
		messageBuffer := make([]byte, numMessageBytes)
		
		// read in the message
		if _, err := io.ReadFull(conn, messageBuffer); err != nil {
			fmt.Println("error reading message into messageBuffer!")
			continue
		}
		
		// evaluate
		var msgContent MessageStruct
		err = json.Unmarshal(messageBuffer, &msgContent)
		if err != nil {
			fmt.Println("error reading in message from client!")
			fmt.Println(err)
		}

		msgTypeRecv, err := strconv.Atoi(msgContent.MsgType)
		if err != nil {
			fmt.Println("problem reading message type!")
			continue
		}
		fmt.Printf("msg type received: %d\n", msgTypeRecv)
		switch msgType := msgTypeRecv
		
		msgType {
			case Hello:
				// when receiving a new user
				fmt.Println("got a hello message! :D")
				
				username := msgContent.Sender
				msg := username + " has joined the server!"
				msgType := strconv.Itoa(Message)
				
				fmt.Println("Going to broadcast: " + msg)
				
				// grab lock
				clientList.mu.Lock()
				
				currentClientNames := []string{username}
				
				for _, connInfo := range clientList.clients {
					// get all connected clients' usernames
					if connInfo.id == clientId {
						connInfo.username = username
					} else {
						currentClientNames = append(currentClientNames, connInfo.username)
					}
				}
				
				// also send the list of current users online
				listOfClientNames := strings.Join(currentClientNames[:], ";")
				
				for _, connInfo := range clientList.clients {
					// tell all connected clients who joined the server (including this client)
					// and include list of all current clients
					conn := connInfo.connection
					msgType = strconv.Itoa(CurrentUsers)
					
					// send new client msg + client names separated by ;
					sendMessage(msg + ";" + listOfClientNames, msgType, conn)
				}
				
				// let go of lock
				clientList.mu.Unlock()
			
			case Message:
				fmt.Println("got a regular message to broadcast!")
				
				clientList.mu.Lock()
				
				for _, connInfo := range clientList.clients {
					clientConn := connInfo.connection
					sendMessage(msgContent.Msg, msgContent.MsgType, clientConn)
				}
				
				clientList.mu.Unlock()
				
			case Goodbye:
				fmt.Println("someone is leaving! :(")
			case CurrentUsers:
				fmt.Println("got an update for current users!")
				
		}
	}

	// TODO: don't close
	//conn.Close()
}

func main() {

	// keep track of connections
	// need to associate a mutex with this cause multithreads
	var clientList ConnectionList

	// print out the ip that's hosting?
	// https://stackoverflow.com/questions/23558425/how-do-i-get-the-local-ip-address-in-go
	port := 2000

	if len(os.Args) > 1 {
		new_port, err := strconv.Atoi(os.Args[1])
		if err == nil {
			port = new_port
		} else {
			fmt.Println(err)
		}
	}
	
	fmt.Println("hello!")
	fmt.Printf("running on port: %d\n", port)

	ln, err := net.Listen("tcp", fmt.Sprintf(":%d", port))

	if err != nil {
		fmt.Printf("there was an error listening on port %s!\n", port)
		fmt.Println(err)
	} else {
		fmt.Println("starting up server...")
	}

	for {
		conn, err := ln.Accept()
		if err != nil {
			fmt.Println("there was problem accepting a connection!")
			fmt.Println(err)
		}
		
		// add to connected clients list
		newId := len(clientList.clients) + 1
		newConn := ConnectionInfo{
			connection: conn,
			id: newId,
			username: "",
		}
		
		// use clientList struct
		clientList.mu.Lock()
		clientList.clients = append(clientList.clients, newConn)
		clientList.mu.Unlock()

		// use a goroutine to handle new connection
		// https://stackoverflow.com/questions/26006856/why-use-the-go-keyword-when-calling-a-function
		go handleConnection(conn, newId, &clientList)
	}

}