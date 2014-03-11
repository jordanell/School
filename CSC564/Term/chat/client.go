package chat

import (
	"code.google.com/p/go.net/websocket"
)

const channelBufSize = 2000

type Client struct {
	ws 			*websocket.Conn
	server 	*Server
	ch 			chan *Message
	done 		chan bool
}

func NewClient(ws *websocket.Conn, server *Server) *Client {

	if ws == nil {
		panic("ws cannot be nil")
	} else if server == nil {
		panic("server cannot be nil")
	}

	ch 		:= make(chan *Message, channelBufSize)
	done 	:= make(chan bool)

	return &Client{ws, server, ch, done}
}

func (self *Client) Conn() *websocket.Conn {
	return self.ws
}

func (self *Client) Write() chan<-*Message {
	return (chan<-*Message)(self.ch)
}

func (self *Client) Done() chan<-bool {
	return (chan<-bool)(self.done)
}

func (self *Client) Listen() {
	go self.listenWrite()
	self.listenRead()
}

func (self *Client) listenWrite() {
	for {
		select {
			// send message to the client
			case msg := <-self.ch:
				websocket.JSON.Send(self.ws, msg)

			// receive done request
			case <-self.done:
				self.server.RemoveClient() <- self
				self.done <- true // for listenRead method
				return
			}
	}
}

func (self *Client) listenRead() {
	for {
		select {
			// receive done request
			case <-self.done:
				self.server.RemoveClient() <- self
				self.done <- true // for listenWrite method
				return

			// read data from websocket connection
			default:
				var msg Message
				err := websocket.JSON.Receive(self.ws, &msg)
				if err != nil {
					self.done<-true
				} else {
					self.server.SendAll() <- &msg
				}
		}
	}
}
