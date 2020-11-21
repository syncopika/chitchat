package main

import (
	"bufio"
	"fmt"
	"io"
	"net"
	"os"
	"strconv"
)

const (
	Hello = iota + 1 // 1
	Message          // 2
	Goodbye          // 3
)

// https://opensource.com/article/18/5/building-concurrent-tcp-server-go
func handleConnection(conn net.Conn) {
	//fmt.Printf("Serving: %s\n", conn.RemoteAddr().String())
	fmt.Printf("got a client!\n");
	
	for {

		data := bufio.NewReader(conn)
		
		// TODO: is this buffer size ok?
		buf := make([]byte, 1024)
		
		// step 1: read in the message type
		if _, err := io.ReadAtLeast(data, buf, 1); err != nil {
			fmt.Println("error reading from the stream! could not get message type.")
			fmt.Println(err)
			break
		}
		
		// evaluate
		fmt.Printf("msg type received: %d\n", buf[0])
		switch msgType := buf[0]
		
		msgType {
			case Hello:
				fmt.Println("got a hello message! :D")
			case Message:
				fmt.Println("got a regular message to broadcast!")
			case Goodbye:
				fmt.Println("someone is leaving! :(")
		}
		
		// step 2: read in the size of the message
		
		// step 3: read in the message
	}

	// TODO: don't close
	//conn.Close()
}

func main() {

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

		// use a goroutine to handle new connection
		// https://stackoverflow.com/questions/26006856/why-use-the-go-keyword-when-calling-a-function
		go handleConnection(conn)
	}

}