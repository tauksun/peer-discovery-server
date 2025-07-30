# Message format 
    action:key1:value1:key2:value2:key3:value3

- Each message has words seprated by colon (:)
- First word defines action
    - action can be : register, register-response, chat-message ... ( exhaustive list yet to be defined )
- From index 1, all words at odd index are "keys" & the following value at even index is "value" to the "key"

## Example message while user registers to discovery server
    register:username:monsterzilla
    
    




