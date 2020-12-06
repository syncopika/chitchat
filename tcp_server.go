package main

import (
	//"bufio"
	"fmt"
	//"io"
	"net"
	"os"
	"strconv"
	"strings"
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
}

func dissectMessage(msg string) []string {
	return strings.Split(msg, ":")
}

// https://opensource.com/article/18/5/building-concurrent-tcp-server-go
func handleConnection(conn net.Conn) {

	// additionally, pass in as an arg the list of client connections
	// grab the mutex and read from the conn in the for loop below
	// each iteration check the length of the list. if diff from last iteration,
	// read all the users and send to client list of users to update UI
	// but also! when broadcasting new messages, we need to send that to all connections
	// in the list!

	//fmt.Printf("Serving: %s\n", conn.RemoteAddr().String())
	fmt.Printf("got a client!\n");
	
	for {
		//data := bufio.NewReader(conn)
		
		// TODO: is this buffer size ok?
		buf := make([]byte, 1024)
		
		/* step 1: read in the message type
		if _, err := io.ReadAtLeast(data, buf, 1); err != nil {
			//fmt.Println("error reading from the stream! could not get message type.")
			//fmt.Println(err)
			//break
			continue
		}*/
		
		numBytesRead, err := conn.Read(buf)
		if err != nil {
			continue
		}
		msgString := string(buf)
		
		fmt.Printf("%d num bytes read.\n", numBytesRead)
		fmt.Println("the message: " + msgString)
		
		// evaluate
		// TODO: we really need to refactor to ensure we get all the bytes in the message!
		firstByte, err := strconv.Atoi(string(msgString[0])) // get int from ascii so we can compare with enum
		if err != nil {
			fmt.Println("problem reading first byte of buffer!")
			continue
		}
		
		fmt.Printf("msg type received: %d\n", firstByte)
		switch msgType := firstByte
		msgType {
			case Hello:
				fmt.Println("got a hello message! :D")
				
				tokens := dissectMessage(msgString) // this is not a good protocol
				if len(tokens) != 3 {
					fmt.Println("message from buffer does not have 3 parts! :(")
				} else {
				
					username := tokens[2]
				
					// write to socket
					msg := "hello there " + username + "!"
					msg = strconv.Itoa(Message) + ":" + strconv.Itoa(len(msg)) + ":" + msg
					conn.Write([]byte(msgString))
					
					// new user has joined. need to let everyone know
				}
			case Message:
				fmt.Println("got a regular message to broadcast!")
				
				// add timestamp to message? in unix timestamp form?
				// need to send to all users
				conn.Write([]byte(msgString))
				
			case Goodbye:
				fmt.Println("someone is leaving! :(")
			case CurrentUsers:
				fmt.Println("got an update for current users!")
		}
		
		// step 2: read in the size of the message
		// step 3: read in the message
	}

	// TODO: don't close
	//conn.Close()
}

func main() {

	// keep track of connections
	// need to associate a mutex with this cause multithreads
	var clients []ConnectionInfo

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
		newId := len(clients) + 1
		newConn := ConnectionInfo{
			connection: conn,
			id: newId,
		}
		clients = append(clients, newConn) // don't forget to pass pointer to clients to new goroutine

		// use a goroutine to handle new connection
		// https://stackoverflow.com/questions/26006856/why-use-the-go-keyword-when-calling-a-function
		go handleConnection(conn)
	}

}