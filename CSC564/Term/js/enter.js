$(document).ready(function() {
	$('#messages-wrapper').scrollTop(('#messages-wrapper')[0].scrollHeight)

	$('#chatInput').bind("keyup keypress", function(e) {
	  var code = e.keyCode || e.which; 
	  if (code  == 13) {               
	    $("#send-btn").click();
	    $('#messages-wrapper').scrollTop(('#messages-wrapper')[0].scrollHeight)
	  }
	});

	$("#send-btn").on('click', function(e) {
		console.log("SEND")
		$('#messages-wrapper').scrollTop(('#messages-wrapper')[0].scrollHeight)
	});
});