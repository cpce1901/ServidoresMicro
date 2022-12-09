var connection = new WebSocket('ws://' + location.hostname + ':81/');

var estado_boton = 0;
var dia = 19;
var mes = 1;
var ano = 1991;
var hora = 10;
var minuto = 10;
var segundo = 0;

var full_fecha = "";
var full_hora = "";

connection.onmessage = function (event) {

    var datos_sensores = event.data;
    var data = JSON.parse(datos_sensores);
    dia = data.day;
    mes = data.month;
    ano = data.year;
    hora = data.hrs;
    minuto = data.minute;
    segundo = data.secon;

    full_fecha = dia + "/" + mes + "/" + ano;
    full_hora = hora + ":" + minuto + ":" + segundo;

    document.getElementById("fecha").innerHTML = full_fecha;
    document.getElementById("hora").innerHTML = full_hora;

}

function button_on() {
    estado_boton = 1;
    console.log("Led is ON");
    send_data();
}

function button_off() {
    estado_boton = 0;
    console.log("Led is OFF");
    send_data();
}

function isChecked() {
    if (document.getElementById('rele_s').checked) {
        estado_boton = 1;
        console.log("Led is ON");
        send_data();
    }
    else {
        estado_boton = 0;
        console.log("Led is OFF");
        send_data();
    }
}

function send_data() {
    var led_estado = '{"Led" :' + estado_boton + '}';
    connection.send(led_estado);
}



