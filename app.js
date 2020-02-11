function register_user() {
    event.preventDefault();
    var username = $("#register-username").val();
    var password = $("#register-password").val();
    console.log(username +" "+ password);
    $.ajax({
        type: 'POST',
        url: "/register_user",
        data: JSON.stringify({"username":username, "password":password})
    }).done(function(data) { 
        alert(data);
    });
}

function populate_registered_user_data() {

}

$( document ).ready(function() {
    
});