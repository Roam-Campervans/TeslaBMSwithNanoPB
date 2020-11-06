# Plans to load Tesla Module status to cloud
- Protocol Buffer to relay information 
- cURLpp to send data to database

# ToDo
- Access to menu calls via HTTP get request
- Access to current settings 
- Auto update voltage on change of > .09
- Alarm if below 15%

# Flow of Collin80's app
-
-
-
-


#  Flow bettween BMS App Java page and Flutter



# What information do I want to send?
- each pack should send the pack data to the SD card and then update the Database if there is service
    - Q how do I determine if service 
        1. get data?
        2. measure cell signal?
    - Data sent
        1. Pack Name(need to add an input to the menu for Pack name)
        2. Module data 
            - voltage and temp of module
            - voltage and temp of each cell in module
            - 