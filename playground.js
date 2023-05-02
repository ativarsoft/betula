$('#betula-form').submit(function(event) {
  event.preventDefault(); // Prevent default form submission behavior

  // Get form data
  var formData = {
    'script': $('textarea[name=betula-code]').val(),
    'template': $('#template').val(),
    'output': $('#output').val()
  };

  // Convert form data to url-encoded query string
  var queryString = $.param(formData);
  // Send Ajax POST request with url-encoded query string
  $.ajax({
    type: 'POST',
    url: 'playground.tmpl',
    data: queryString,
    contentType: 'application/x-www-form-urlencoded',
    dataType: 'html',
    encode: true
  })
  // Handle successful response
  .done(function(data) {
    // Display response in result div
    $('#result').html('<pre>' + data + '</pre>');
  })
  // Handle error response
  .fail(function(data) {
    // Display error message in result div
    $('#result').html('<p>Error: ' + data.response + '</p>');
  });
});
