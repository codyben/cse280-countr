function get_tuple(d) {
    var keys = [];
    var vals = [];
    for(var k in d){ 
        keys.push(k);
        vals.push(d[k]);
    }
    return [keys, vals];
}

function on_login() {
    var register_a = $(".register a");
    register_a.text("Logout");
    var login_a = $(".login a");
    login_a.text("Hi, "+Cookies.get("username"));
    login_a.attr("href", "");
    register_a.attr("href", "");
    register_a.attr("onclick", "logout()");
    var create_a = $(".create");
    create_a.text("make a countr");
    create_a.attr("href", "#create_counter");
    create_a.attr("rel", "modal:open");
}

function logout() {
    Cookies.set("username","");
    Cookies.set("session", "");
    var register_a = $(".register a");
    register_a.text("Register");
    register_a.attr("href", "#register_modal");
    var login_a = $(".login a");
    login_a.text("Login");
    login_a.attr("href", "#login_modal");
    register_a.attr("onclick", "");
    var create_a = $(".create");
    create_a.text("countr");
    create_a.attr("href", "");
    create_a.attr("rel", "");
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
    var username = Cookies.get("username");
    var session = Cookies.get("session");
    var amt = $("#counter-amt").val();
    if(!amt) {
        amt = 2;
    }

    // console.log(username +" "+ password);
    $.ajax({
        type: 'POST',
        url: "/create_counter",
        data: JSON.stringify({"username":username, "initial_count":amt, "session_key": session})
    }).done(function(data) { 
        data = JSON.parse(data);
        if( "error" in data) {
            alert(data.error);
        } else {
            get_user_counter();
        }
    });
}

function increment_counter() {
    event.preventDefault();
    var username = $("#login-username").val();
    var password = $("#login-password").val();
    console.log(username +" "+ password);
    $.ajax({
        type: 'GET',
        url: "/increment_count",
        data: JSON.stringify({"username":username, "counter_id":password})
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
        url: "/decrement_count",
        data: JSON.stringify({"username":username, "password":password})
    }).done(function(data) {
        data = JSON.parse(data); 
        console.log(data);
    });
}

function get_user_counter() {
    event.preventDefault();
    var username = Cookies.get("username");
    if(!username) {
        alert("Sorry, I can't do that");
    } else {
    // console.log(username +" "+ password);
        $.ajax({
            type: 'POST',
            url: "/get_user_counters",
            data: JSON.stringify({"username":username}),
            async: false,
            success: function(data) {
                console.log(data);
            }
        });
    }
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

function test_me() {
    alert("find me");
}

$( document ).ready(function() {
    check_if_we_logged_in_bro();
});