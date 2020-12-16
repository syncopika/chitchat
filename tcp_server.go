package main

import (
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

func dissectMessage(msg string) []string {
	return strings.Split(msg, ":")
}

func sendMessage(msg string, msgType string, conn net.Conn) {

	timestamp := strconv.FormatInt(time.Now().Unix(), 10)
	
	// write to socket
	msg = msgType + ":" + timestamp + ": " + msg

	// get length of msg in bytes! (since num of chars in string != num of bytes)
	msgLength := len([]byte(msg))
	
	// prepend msg with the length of msg so receiver knows how many bytes to read
	conn.Write([]byte(string(msgLength) + msg))
}

// https://opensource.com/article/18/5/building-concurrent-tcp-server-go
func handleConnection(conn net.Conn, clientId int, clientList *ConnectionList) {

	//fmt.Printf("Serving: %s\n", conn.RemoteAddr().String())
	fmt.Printf("got a client!\n");
	
	for {
		// this is to get the message length (which should be one byte)
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
		
		msgString := string(messageBuffer)
		fmt.Println("the message: " + msgString)
		fmt.Printf("message length: %d\n", len(msgString))
		
		// evaluate
		firstByte, err := strconv.Atoi(string(msgString[0])) // get int from ascii so we can compare with enum
		if err != nil {
			fmt.Println("problem reading first byte of buffer!")
			continue
		}
		
		fmt.Printf("msg type received: %d\n", firstByte)
		switch msgType := firstByte
		msgType {
			case Hello:
				// when receiving a new user
				fmt.Println("got a hello message! :D")
				
				tokens := dissectMessage(msgString) // this is not a good protocol
				if len(tokens) != 3 {
					fmt.Println("message from buffer does not have 3 parts! :(")
				} else {
					username := tokens[2]
					msg := username + " has joined the server!"
					msgType := strconv.Itoa(Message)
					
					fmt.Println("Going to broadcast: " + msg)
					
					clientList.mu.Lock()
				
					currentClientNames := []string{username}
					
					for _, connInfo := range clientList.clients {
						// tell all connected clients who joined the server (including this client)
						
						clientConn := connInfo.connection
						sendMessage(msg, msgType, clientConn)
						
						// make sure to update username in this client's connInfo
						if connInfo.id == clientId {
							connInfo.username = username
						} else {
							currentClientNames = append(currentClientNames, connInfo.username)
						}
					}
					
					// also send the list of current users online
					listOfClientNames := strings.Join(currentClientNames[:], ";")
					fmt.Println("Going to broadcast: " + listOfClientNames)
					
					for _, connInfo := range clientList.clients {
						conn := connInfo.connection
						msgType = strconv.Itoa(CurrentUsers)
						sendMessage(listOfClientNames, msgType, conn)
					}
					
					clientList.mu.Unlock()
				}
			case Message:
				fmt.Println("got a regular message to broadcast!")
				
				// extract the message and reformat it
				msgParts := dissectMessage(msgString)
				msg := msgParts[1] + " - " + msgParts[2]
				msgType := strconv.Itoa(Message)
				
				clientList.mu.Lock()
				
				for _, connInfo := range clientList.clients {
					clientConn := connInfo.connection
					sendMessage(msg, msgType, clientConn)
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