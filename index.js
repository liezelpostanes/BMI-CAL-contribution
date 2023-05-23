function countBmi() {
  var age = parseInt($('#age').val());
  var height = parseInt($('#height').val());
  var weight = parseInt($('#weight').val());
  var gender = $('input[name=gender]:checked').val();

  // Validate input
  if (isNaN(age) || isNaN(height) || isNaN(weight) || !gender) {
    alert('All fields are required!');
    return;
  }

  // Calculate BMI
  var bmi = weight / ((height / 100) ** 2);

  var result = '';
  if (bmi < 18.5) {
    result = 'Underweight';
  } else if (bmi >= 18.5 && bmi <= 24.9) {
    result = 'Healthy';
  } else if (bmi >= 25 && bmi <= 29.9) {
    result = 'Overweight';
  } else if (bmi >= 30 && bmi <= 34.9) {
    result = 'Obese';
  } else if (bmi >= 35) {
    result = 'Extremely obese';
  }

  var h1 = document.createElement("h1");
  var h2 = document.createElement("h2");

  var t = document.createTextNode(result);
  var b = document.createTextNode('BMI: ');
  var r = document.createTextNode(bmi.toFixed(2));

  h1.appendChild(t);
  h2.appendChild(b);
  h2.appendChild(r);

  document.body.appendChild(h1);
  document.body.appendChild(h2);
}
  });

    // Send the AJAX request
    $.ajax({
      url: "cgi-bin/post.o",
      type: "POST",
      data: data,
      contentType: "application/json",
      success: function(response) {
        // Handle the response
        var result = JSON.parse(response);
        var bmi = result.bmi.toFixed(2);
        var category = result.category;
        var resultText = "BMI: " + bmi + " - " + category;

        $('#result').text(resultText);
      },
      error: function(xhr, status, error) {
        console.error("AJAX request error:", status, error);
      }
    });
  });
});
