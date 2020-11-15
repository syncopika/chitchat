package main

import "bufio"
import "fmt"
import "net"
import "strings"

// https://opensource.com/article/18/5/building-concurrent-tcp-server-go
func handleConnection(conn net.Conn) {
	fmt.Printf("Serving: %s\n", conn.RemoteAddr().String())
	
	for {
		data, err := bufio.NewReader(conn).ReadString('\n')
		if err != nil {
			fmt.Println(err)
			return
		}
		
		msg := strings.TrimSpace(string(data))
		fmt.Printf("msg received: %s\n", msg)
	}

	conn.Close()
}

func main() {

	port := 2000

	ln, err := net.Listen("tcp", fmt.Sprintf(":%d", port))

	if err != nil {
		fmt.Printf("there was an error listening on port %s!", port)
		fmt.Println(err)
	}else{
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