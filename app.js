function on_login() {
    var register_a = $(".register a");
    register_a.text("Logout");
    var login_a = $(".login a");
    login_a.text("Hi, "+Cookies.get("username"));
    login_a.attr("href", "");
    register_a.attr("href", "");
    register_a.attr("onclick", "logout()");
}

function logout() {
    Cookies.get("username","");
    Cookies.get("session", "");
    var register_a = $(".register a");
    register_a.text("Register");
    register_a.attr("href", "#register_modal");
    var login_a = $(".login a");
    login_a.text("Login");
    login_a.attr("href", "#login_modal");
    register_a.attr("onclick", "");
}

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
        data = JSON.parse(data);
        if("error" in data) {
            alert(data.error);
        } else {
            Cookies.set("session", data.key);
            Cookies.set("username", data.username);
            on_login();
        }
    });
}

function login_user() {
    event.preventDefault();
    var username = $("#login-username").val();
    var password = $("#login-password").val();
    console.log(username +" "+ password);
    $.ajax({
        type: 'POST',
        url: "/login_user",
        data: JSON.stringify({"username":username, "password":password})
    }).done(function(data) { 
        data = JSON.parse(data);
        if("error" in data) {
            alert(data.error);
        } else {
            Cookies.set("session", data.key);
            Cookies.set("username", data.username);
            on_login();
        }
    });
}

function create_counter() {
    event.preventDefault();
    var username = $("#login-username").val();
    var password = $("#login-password").val();
    console.log(username +" "+ password);
    $.ajax({
        type: 'GET',
        url: "/create_counter",
        data: JSON.stringify({"username":username, "password":password})
    }).done(function(data) { 
        data = JSON.parse(data);
        alert(data);
    });
}

function increment_counter() {
    event.preventDefault();
    var username = $("#login-username").val();
    var password = $("#login-password").val();
    console.log(username +" "+ password);
    $.ajax({
        type: 'GET',
        url: "/increment_counter",
        data: JSON.stringify({"username":username, "password":password})
    }).done(function(data) { 
        data = JSON.parse(data);
        alert(data);
    });
}

function decrement_counter() {
    event.preventDefault();
    
    console.log(username +" "+ password);
    $.ajax({
        type: 'GET',
        url: "/decrement_counter",
        data: JSON.stringify({"username":username, "password":password})
    }).done(function(data) {
        data = JSON.parse(data); 
        console.log(data);
    });
}

function get_user_counter() {
    event.preventDefault();
    var username = $("#get-counter-username").val();
    console.log(username +" "+ password);
    $.ajax({
        type: 'GET',
        url: "/get_user_counters",
        data: JSON.stringify({"username":username})
    }).done(function(data) {
        data = JSON.parse(data); 
        alert(data);
    });
}

function check_if_we_logged_in_bro() {
    var c_key = Cookies.get("session");
    var u = Cookies.get("username");

    if(u && c_key) {
        on_login();
    }
}

function populate_registered_user_data() {

}

$( document ).ready(function() {
    check_if_we_logged_in_bro();
});