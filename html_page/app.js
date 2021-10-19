

var address = "http://localhost:1234";

function volUp()
{
    sendCommand("VOL_UP");
}

function volDown()
{
    sendCommand("VOL_DOWN");
}

function startShowing()
{
    sendCommand("START_SHOWING");
}

function stopShowing()
{
    sendCommand("STOP_SHOWING");
}

function pause()
{
    sendCommand("PASUE");
}

function play()
{
    sendCommand("PLAY");
}

function lightDown()
{
    sendCommand("LIGHT_DOWN");
}

function lightUp()
{
    sendCommand("LIGHT_UP");
}

function rollerUnfold()
{
    sendCommand("ROLLER_UNFOLD");
}

function rrollerFold()
{
    sendCommand("ROLLER_FOLD");
}

function sendCommand(command)
{
    let obj = new Object();
    obj.command = command;
    let jsonString= JSON.stringify(obj);
    console.log(jsonString);
    sendHttpPostRequest(address, jsonString);
}


function sendHttpPostRequest(address, commandJson)
{
    var xhr = new XMLHttpRequest();
    xhr.open("POST", address, true);
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.send(commandJson);
}