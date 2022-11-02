var webSocket, dataPlot;
var estado_boton = 0;
var temperatura_1 = 0;
var humedad_1 = 0;
var procio_1 = 0;
var temperatura_2 = 0;
var humedad_2 = 0;
var procio_2 = 0;
var temperatura_3 = 0;
var humedad_3 = 0;
var procio_3 = 0;
var temperatura_r = 0;
var humedad_r = 0;
var procio_r = 0;
var contador = 0;

//lecturas

const tp = [];
const hp = [];
const prp = [];

//Datos
const maxDataPoints = 20;
 const data = {
  labels: [],
  datasets: [{
    data: tp,
    label: "Temperatura Promedio",
    borderColor: '#fd7e14',
    fill: false
  },
  {
    data: hp,
    label: "Humedad Promedio",
    borderColor: '#1b99e0',
    fill: false
  },
  {
    data: prp,
    label: "Punto Rocío Promedio",
    borderColor: '#1b78e2',
    fill: false
  }]
}



//configuracion grafico
const config = {
  type: 'line',
  data: data,
  options: {
    scales:{
      x: {
        ticks:{
          callback: function(value){
            return this.getLabelForValue(value).substr(11);
          }
        }
      }

    },
    maintainAspectRatio: false,
    plugins: {
      title: {
        display: true,
        text: 'Temperatura Promedio - Humedad Promedio - Punto Rocío Promedio'
      }
    }    
  },
};




function init(){
var connection = new WebSocket('ws://'+location.hostname+':81/');
var dataPlot = new Chart(linea,
  config
  );  

  function removeData(){
    dataPlot.data.labels.shift();
    dataPlot.data.datasets[0].data.shift();
  }
  
  function addData (label, data1, data2, data3){
    if(dataPlot.data.labels.length >= maxDataPoints) removeData();
    dataPlot.data.labels.push(label);  
    tp.push(data1.toFixed(2));
    hp.push(data2.toFixed(2));
    prp.push(data3.toFixed(2));
    dataPlot.update();
    
  }

  function hora(){
    var date = new Date();
    var fecha = String(date.getDate()).padStart(2, '0') + '/' + String(date.getMonth() + 1).padStart(2, '0') + '/' + date.getFullYear();
    var hora = String(date.getHours()).padStart(2, '0') + ':' + String(date.getMinutes()).padStart(2,'0') + ':' + date.getSeconds();
    var output = fecha + ' ' + hora;
    return output;
  }
  

connection.onmessage = function(event){   

  var datos_sensores = event.data;
  var data = JSON.parse(datos_sensores);  
        
  temperatura_1 = data.temp1;
  humedad_1 = data.hum1;
  procio_1 = data.procio1;
    
  temperatura_2 = data.temp2;
  humedad_2 = data.hum2;
  procio_2 = data.procio2;
    
  temperatura_3 = data.temp3;
  humedad_3 = data.hum3;
  procio_3 = data.procio3;
    
  temperatura_r = (data.temp1 + data.temp2 + data.temp3)/3;
  humedad_r = (data.hum1 + data.hum2 + data.hum3)/3;
  procio_r = (data.procio1 + data.procio2 + data.procio3)/3; 
    
  
  document.getElementById("temp_1_value").innerHTML = temperatura_1; 
  document.getElementById("hum_1_value").innerHTML = humedad_1;
  document.getElementById("procio_1_value").innerHTML = procio_1; 
 
  document.getElementById("temp_2_value").innerHTML = temperatura_2;
  document.getElementById("hum_2_value").innerHTML = humedad_2;
  document.getElementById("procio_2_value").innerHTML = procio_2;

  document.getElementById("temp_3_value").innerHTML = temperatura_3;
  document.getElementById("hum_3_value").innerHTML = humedad_3;
  document.getElementById("procio_3_value").innerHTML = procio_3;
    
  document.getElementById("temp_r_value").innerHTML = temperatura_r.toFixed(2);
  document.getElementById("hum_r_value").innerHTML = humedad_r.toFixed(2);
  document.getElementById("procio_r_value").innerHTML = procio_r.toFixed(2);       
  
  contador ++;

  if (contador == 1 || contador % 12 == 0 ){
    var output2 = hora();  
    addData(output2, temperatura_r, humedad_r, procio_r)   
  }  
}

}
