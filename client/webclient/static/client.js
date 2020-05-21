var nodes, edges, network, socket, svgRoot;

$(document).ready(function() {

  svgRoot = null;

  var mmi = document.getElementById("mmi_svg");

  //it's important to add an load event listener to the object, as it will load the svg doc asynchronously
  mmi.addEventListener("load",function(){
      var svgDoc = mmi.contentDocument; //get the inner DOM of alpha.svg
      svgRoot  = svgDoc.documentElement;
  },false);

	//add tabs
	$("#hostlogtab").dynatabs({
		tabBodyID : "hostlogbody",
		showCloseBtn : true,
	});
  var tabje = $("#localhost_tab")[0];
  tabje.children[0].classList.remove("closeable");

  namespace = '';
  socket = io.connect('http://' + document.domain + ':' + location.port + namespace);

  /* register events from server */

  //add info to the ied/datamodel tab
  socket.on('info_event', function (data) {
    //event gets called from server when info data is updated, so update the info tab
    //write data
    var tp = data['type'];
    if(tp == '0'){
        $('#datamodel')[0].innerHTML = data['data'];
    }
  });

  //add info to the ied/datamodel tab
  socket.on('svg_value_update_event', function (data) {
    //event gets called from server when svg data is updated, so update the svg
    var value = data['value'];
    var element = data['element'];
    var type = data['type'];

    if(svgRoot != null){//if the svg is loaded
      //var aa = $("#svg_147",svgRoot);
      //aa[0].textContent = "hoi";
      if(type == 'text'){
        $("#" + $.escapeSelector(element),svgRoot)[0].textContent = value;
      }
      if(type == 'switch'){
        var el = $("#" + $.escapeSelector(element),svgRoot)[0];
        if(value=='open'){
          $("#open",el)[0].beginElement();
        }
        else{
          $("#close",el)[0].beginElement();
        }
      }
    }
  });

  //send by server when a log-line should be added to a tab
  //if the tab did not exist yet, create it
  socket.on('log_event', function (data) {
    //event gets called from server when new log events are generated, and add them to the log tab
    //if clear is set, all log data is cleared before adding the new data
    if($("#" + data['host'] + "_tab").length == 0) {
      //addNode(data['host'], '1');
      addNewStaticTab(data['host']);
    }

    var ahref = $("#" + data['host'] + "_tab")[0];
    var key = $(ahref).attr('href');
    if(data['clear'] == '1') {
      $(key)[0].innerHTML = "<pre>" + data['data'] + "</pre>";  
    }
    else {
      $(key)[0].innerHTML += "<pre>" + data['data'] + "</pre>";
    }
    //scroll to bottom
    var element = $("#hostlogbody")[0];
    element.scrollTop = element.scrollHeight;
  });

  //send by server when a different logging tab should be selected
  socket.on('select_tab_event', function (data) {
    //event gets called from server when the tab-focus should be changed
    var tab = $('#hostlogtab.tabs');
    var ahref;
    if(data['host_index']){
      var integer = parseInt(data['host_index'], 10);
      ahref = $('#hostlogtab.tabs')[0].children[integer].children[0];
    }
    else if(data['host_name']){
      ahref = $("#" + data['host_name'] + "_tab")[0];
    }
    selectTabByHref(tab, ahref);
  });


  socket.on('page_reload', function (data) {
    location.reload();
  });

});

/********************************************************/
/*             socket.io calls               */
/********************************************************/

/* emit from client to server */

function get_page_data() {
  socket.emit('get_page_data', {data: ''});
  //call server to tell we want all data, so we can fill the ui (normally done once on full page load/refresh)
}

function start_simulation() {
  socket.emit('start_simulation', '');
}

function stop_simulation() {
  socket.emit('stop_simulation', '');
}


/********************************************************/
/*                        UI calls                      */
/********************************************************/

//add loggin tab based on hostname
function addNewStaticTab(host)
{
  var ret = 0;
  if($("#" + host + "_tab").length > 0) {
    console.log("tab exists");
    return 1;//tab exists
  }

  $.addDynaTab({
    tabID : 'hostlogtab',
    type : 'html',
    html : '<pre>[+] logging started</pre>',
    tabTitle : host
  });

  //add hostname to button-id
  try {
    var len = $('#hostlogtab.tabs')[0].children.length;
    var ahref = $('#hostlogtab.tabs')[0].children[len-1].children[0];
    ahref.id = host + "_tab";

    //add additional callback
    $("#" + host + "_tab").bind('click',{ ahref: ahref, tab: ahref.id, host: host}, myTabClick );	
  }
  catch (err) {
    console.log(err);
    ret = -1;
  }

  return ret;
}

//set focus when a tab is clicked
function myTabClick(event)
{
  socket.emit('set_focus', event.data.host);
}

//select the tab based on the hostname
function selectTabByHref(tab, ahref)
{
  var ret = 0;
  try {
    var event = $.Event("click");
    event.data = {ahref:"",tab:""};
    event.data.ahref = ahref;
    event.data.tab = tab;
    $("#hostlogtab").showTab(event);
  }
  catch (err) {
    console.log(err);
    ret = -1;
  }
  return ret;
}

function writeValue(event){
  alert('write a value');
  socket.emit('write_value', { id : this.id, value : '10' });
}

function writePosition(event){
  socket.emit('write_position', { id : this.id });
}

function draw() {
  // create a network
  var container = document.getElementById('network');
  // load the svg data
  // register the elements via emit call of socketio
  $("g",svgRoot).find("*").each(function(idx, el){
      console.log("id:" + el.id );
      if(el.id.startsWith("ied://") == true){
        var cl = el.classList.toString();
        socket.emit('register_datapoint', {id : el.id, class : cl});
        if(cl == "XCBR"){
          el.onclick = writePosition;
        }
        if(cl == "LOAD"){
          el.onclick = writeValue;
        }
      }
  })
  socket.emit('register_datapoint_finished', '');
  // connect functions for reading/writing values and generating faults, that can socket.emit
}

//register a socket.on for value-updates, that updates the svg element

