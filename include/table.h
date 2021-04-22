
char tableMenu[] PROGMEM = 
R"=====(
<!DOCTYPE html>
<html lang="sv">
<head>
<meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
<style>
table, th, td {
  border: 1px solid black;
}
.button {
        display: inline-block;
        padding: 10px 20px;
        text-align: center;
        text-decoration: none;
        color: #ffffff;
        background-color: #7aa8b7;
        border-radius: 6px;
        outline: none;
      }
</style>
</head>
<body>

<h1>COMPAK ESLÖV</h1>
<table>
  <tr>
    <th>-1-</th>
    <th>-2-</th>
    <th>-3-</th>    
    <th>-4-</th>    
    <th>-5-</th>    
  </tr>
  <tr>
    <td>$@000</td>
    <td>$@001</td>
    <td>$@002</td>
    <td>$@003</td>
    <td>$@004</td>
  </tr>
  <tr>
    <td>$@005</td>
    <td>$@006</td>
    <td>$@007</td>
    <td>$@008</td>
    <td>$@009</td>
  </tr>
  <tr>
    <td>$@010</td>
    <td>$@011</td>
    <td>$@012</td>
    <td>$@013</td>
    <td>$@014</td>
  </tr>
  <tr>
    <td>$@015</td>
    <td>$@016</td>
    <td>$@017</td>
    <td>$@018</td>
    <td>$@019</td>
  </tr>
  <tr>
    <td>$@020</td>
    <td>$@021</td>
    <td>$@022</td>
    <td>$@023</td>
    <td>$@024</td>
  </tr>

</table>
    Click <a href='/'>here</a> go back<br>
     <a class="button" href='/'>Tillbaka</a>  <a class="button" href='/table'>Framåt</a>
</body>
</html>
)=====";
HtmlStruct tableStruct =    {tableMenu, 25, 
                                {
                                    (char*)"A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",
                                    (char*)"A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",
                                    (char*)"A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",
                                    (char*)"A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",
                                    (char*)"A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A",(char*) "A+A"
                                }
                            };
