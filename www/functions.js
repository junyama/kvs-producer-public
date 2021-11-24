var socket;

function connentWebSocket(operation) {
    if (operation == "open") {

        var url = document.getElementById("url").value;
        socket = new WebSocket(url);
        socket.onopen = function (event) {
            writeLog("logId", 'Socket connected');
        };
        socket.onclose = function (event) {
            writeLog("logId", 'Socket closed');
        };
        socket.onmessage = function (event) {
            writeLog("logId", event.data);
        };
        document.getElementById("connecWsId").value = "Disconnect";
        document.getElementById("connecWsId").onclick = function (e) {
            connentWebSocket("close")
        };

    } else {
        socket.close();
        document.getElementById("connecWsId").value = "Connect";
        document.getElementById("connecWsId").onclick = function (e) {
            connentWebSocket("open")
        };
    }
}

function sendMsg() {
    var message = document.getElementById("message").value;
    socket.send(message);
};

function resetTextArea(textAreaId) {
    document.getElementById(textAreaId).innerHTML = "-- beginning of log ---";
}

function writeLog(textAreaId, string) {
    //var contents = document.getElementById(textAreaId).innerHTML;
    //document.getElementById(textAreaId).innerHTML = string + "<br>" + document.getElementById(textAreaId).innerHTML;
    let parent = document.getElementById(textAreaId);
    let child = document.createElement('div');
    child.prepend(string);
    parent.prepend(child);
}

function getSwaggerIp() {
    $.ajax({
        url: '/getSwaggerIp/',
        dataType: 'text',
        success: function (data) {
            document.getElementById("url").value = "ws://" + data + ":8080/ws";
        },
        error: function (data) {
            alert('error to get swaggerIp');
        }
    });

}