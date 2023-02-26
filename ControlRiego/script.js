window.onload = function (event) {
    init();
    show();
}

var Socket;
var estado_boton = 0;
var estado_sensor = 0;
var estateLed = 0;
var estateSensor = 0;
var dia = 19;
var mes = 1;
var ano = 1991;
var hora = 10;
var minuto = 10;
var segundo = 0;
var on_1 = "";
var on_2 = "";
var on_3 = "";
var on_4 = "";
var on_5 = "";
var off_1 = "";
var off_2 = "";
var off_3 = "";
var off_4 = "";
var off_5 = "";


var full_fecha = "";
var full_hora = "";
var full_hora_on = ""
var full_hora_off = ""


function init() {
    Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
    Socket.onmessage = function (event) {
        processCommand(event);
    };
}

function processCommand(event) {
    var data = JSON.parse(event.data);
    console.log(data);

    dia = data.day;
    mes = data.month + 1;
    ano = data.year;
    hora = data.hrs;
    minuto = data.minute;
    segundo = data.secon;

    on_1 = data.on_1;
    on_2 = data.on_2;
    on_3 = data.on_3;
    on_4 = data.on_4;
    on_5 = data.on_5;

    off_1 = data.off_1;
    off_2 = data.off_2;
    off_3 = data.off_3;
    off_4 = data.off_4;
    off_5 = data.off_5;

    estateLed = data.estateLed;
    estateSensor = data.estateSensor;

    if (hora < 10) {
        var new_hora = "0" + String(hora)
    }
    else {
        new_hora = String(hora)
    }

    if (minuto < 10) {
        var new_minuto = "0" + String(minuto)
    }
    else {
        new_minuto = String(minuto)
    }

    if (segundo < 10) {
        var new_segundo = "0" + String(segundo)
    }
    else {
        new_segundo = String(segundo)
    }

    if (dia < 10) {
        var new_dia = "0" + String(dia)
    }
    else {
        new_dia = String(dia)
    }

    if (mes < 10) {
        var new_mes = "0" + String(mes)
    }
    else {
        new_mes = String(mes)
    }

    full_fecha = new_dia + "/" + new_mes + "/" + ano;
    full_hora = new_hora + ":" + new_minuto + ":" + new_segundo;


    document.getElementById("fecha").innerHTML = full_fecha;
    document.getElementById("hora").innerHTML = full_hora;

    document.getElementById("on_1").innerHTML = on_1;
    document.getElementById("on_2").innerHTML = on_2;
    document.getElementById("on_3").innerHTML = on_3;
    document.getElementById("on_4").innerHTML = on_4;
    document.getElementById("on_5").innerHTML = on_5;

    document.getElementById("off_1").innerHTML = off_1;
    document.getElementById("off_2").innerHTML = off_2;
    document.getElementById("off_3").innerHTML = off_3;
    document.getElementById("off_4").innerHTML = off_4;
    document.getElementById("off_5").innerHTML = off_5;


    if (estateLed) {
        document.getElementById('rele_s').checked = 1;
    }
    else {
        document.getElementById('rele_s').checked = 0;
    }

    if (estateSensor) {
        document.getElementById('sensor_s').checked = 1;
    }
    else {
        document.getElementById('sensor_s').checked = 0;
    }


}

function isChecked() {
    if (document.getElementById('rele_s').checked) {
        estado_boton = 1;
        console.log("Led is ON");
        button_send_back();
    }
    else {
        estado_boton = 0;
        console.log("Led is OFF");
        button_send_back();
    }
}

function isChecked2() {
    if (document.getElementById('sensor_s').checked) {
        estado_sensor = 1;
        console.log("Sensor is ON");
        button_send_back();
    }
    else {
        estado_sensor = 0;
        console.log("Sensor is OFF");
        button_send_back();
    }
}

function read_all() {

    let formato = /^(0[1-9]|1\d|2[0-3]):([0-5]\d):([0-5]\d)$/
    var lista_on = [];
    var lista_off = [];

    for (i = 1; i < 6; i++) {
        var dato_on = document.getElementById('on' + String([i])).value;
        if (formato.test(dato_on)) {
            lista_on.push(dato_on);
        }
        else if (dato_on == "") {
            dato_on = "--:--:--";
            lista_on.push(dato_on);
        }
        else if (dato_on == "--:--:--") {
            lista_on.push(dato_on);
        }
        else {
            alert("Formato incompatible, Porfavor revisa");
            break;
        }
    }

    for (i = 1; i < 6; i++) {
        var dato_off = document.getElementById('off' + String([i])).value;
        if (formato.test(dato_off)) {
            lista_off.push(dato_off);
        }
        else if (dato_off == "") {
            dato_off = "--:--:--";
            lista_off.push(dato_off);
        }
        else if (dato_off == "--:--:--") {
            lista_off.push(dato_off);
        }
        else {
            alert("Formato incompatible, Porfavor revisa");
            break;
        }
    }


    on_1 = lista_on[0];
    on_2 = lista_on[1];
    on_3 = lista_on[2];
    on_4 = lista_on[3];
    on_5 = lista_on[4];

    off_1 = lista_off[0];
    off_2 = lista_off[1];
    off_3 = lista_off[2];
    off_4 = lista_off[3];
    off_5 = lista_off[4];

    show();
    button_send_back();
    

}

function button_send_back() {
    var msg = {
        on1: on_1,
        on2: on_2,
        on3: on_3,
        on4: on_4,
        on5: on_5,
        off1: off_1,
        off2: off_2,
        off3: off_3,
        off4: off_4,
        off5: off_5,
        led: estado_boton,
        sen: estado_sensor
    };
    Socket.send(JSON.stringify(msg));

}

function show() {
    var x = document.getElementById("edit");
    if (x.style.display === "none") {
        x.style.display = "block";
        showAll();
    } else {
        x.style.display = "none";

    }

}

function showAll() {

    document.getElementById("on1").value = on_1;
    document.getElementById("on2").value = on_2;
    document.getElementById("on3").value = on_3;
    document.getElementById("on4").value = on_4;
    document.getElementById("on5").value = on_5;

    document.getElementById("off1").value = off_1;
    document.getElementById("off2").value = off_2;
    document.getElementById("off3").value = off_3;
    document.getElementById("off4").value = off_4;
    document.getElementById("off5").value = off_5;
}

