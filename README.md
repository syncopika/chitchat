# chitchat    
    
yet another chat application made with Qt (and Go for a demo server)    
based off of: https://github.com/syncopika/boringChat    
currently a work-in-progress and only sort of functional (with a lot more work to be done)    
    
current look:    
![the login page](screenshots/loginPage.png)    
    
![the chat page](screenshots/chatArea.png)    
    
![the emoticon edit page](screenshots/emoticonEditPage.png)    
    
![the go server](screenshots/go-server.png)    
    
A key feature is being able to have complex ASCII/unicode emoticons on hand and ready for use! for when emojis just aren't good enough :)    
    
## TODO:    
- change message parsing i.e. don't split on colons (a lot of emoticons use those lol)
- make emoticon edit page/widget functional
- implement disconnect
- loading emoticons from a saved json?
- testing with multiple users
- implement avatar feature