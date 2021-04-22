
char htmlMainMenu[] PROGMEM = 
R"=====(
<html>    
    Status Relay: <span style='color:$@000;'><strong>$@001</strong></span>
    <h1>Actions: </h1>
    <ul style='line-height:180%'>
        <li><a href='/pump_relay_on'>START Pump Relay</a></li>
        <li><a href='/pump_relay_off'>STOP Pump Relay</a></li>
    </ul>
        Click <a href='/pump_relay_on'>here</a> turn the built in LED on<br>
        Click <a href='/drop'>here</a> go to dropdown<br>
        Click <a href='/table'>here</a> go to table<br>

</html>
)=====";

HtmlStruct mainMenuStruct = {htmlMainMenu, 2, {(char*)"red",(char*) "OFF"}};
