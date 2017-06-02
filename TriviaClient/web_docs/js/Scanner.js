function Scanner(ip, port) {
    var socket = new WebSocket("ws://" + ip + ":" + port + "/");

    var messageQueue = [];
    var handlers = {};
    var temps = {};
    var immediateHandler = null;
    var bytesRequired = null;

    var loadiv, readiv;

    socket.onmessage = function(m) {
        var data = isNaN(m.data) ? m.data : parseInt(m.data);
        console.log("Got packet: " + data);
        if (data == NaN) {
            debugger;
        }
        //console.log("Scanner received " + data + ", queue length " + messageQueue.length);
        if (immediateHandler) {
            //console.log(data + " is going to be queued.");
            messageQueue.push(data);
            if (messageQueue.length >= bytesRequired && immediateHandler) {
                var a = immediateHandler;
                immediateHandler = null;
                bytesRequired = null;
                var b = messageQueue;
                messageQueue = [];
                //console.log("Queue is being executed asynchronously");
                a(b);
            }
        }
        else if (temps[data]) {
            //console.log(data + " is going to a temporary callback handler");
            temps[data]();
            delete temps[data];
        }
        else if (handlers[data]) {
            //console.log(data + " is going to a persnstent callback handler");
            handlers[data]();
        }
        else {
            
        }
    };
    socket.onopen = function() {
        loadiv.hide();
        readiv.show();
    };
    socket.onerror = function() {
        $(".full").hide();
        $("#failedConnectionDiv").show();
        socket.close();
    };
    socket.onclose = function() {
        $(".full").hide();
        $("#failedConnectionDiv").show();
    }
    this.send = function(e) {
        socket.send(e.toString());
        return this;
    };
    this.callback = function(code, handler) {
        temps[code] = handler;
        return this;
    };
    this.handler = function(code, handler) {
        handlers[code] = handler;
        return this;
    };
    this.expect = function(bytes, callback) {
        //console.log("expecting " + bytes + " bytes");
        if (messageQueue.length >= bytes) {
            console.log("Queue is being executed synchronously");
            callback(messageQueue);
            messageQueue = [];
            return;
        }
        bytesRequired = bytes;
        immediateHandler = callback;
        return this;
    };
    this.loading = function(l) {
        loadiv = l;
        return this;
    };
    this.ready = function(l) {
        readiv = l;
        return this;
    };
}