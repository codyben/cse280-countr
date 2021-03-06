function get_tuple(d) {
    var keys = [];
    var vals = [];
    for(var k in d){ 
        keys.push(k);
        vals.push(d[k]);
    }
    return [keys, vals];
}

function close_all_modals() {
    $(".close_me").click();
}

function refresh() {
    $(".pure-g").empty();
    get_user_counter();
}

function on_login() {
    refresh();
    close_all_modals();
    var register_a = $(".register a");
    register_a.text("Logout");
    var login_a = $(".login a");
    login_a.text("Hi, "+Cookies.get("username"));
    login_a.attr("href", "");
    login_a.attr("rel", "");
    register_a.attr("href", "");
    register_a.attr("onclick", "logout()");
    var create_a = $(".create");
    create_a.text("make a countr");
    create_a.attr("href", "#create_counter");
    create_a.attr("rel", "modal:open");
}

function logout() {
    $(".pure-g").empty();
    Cookies.set("username","");
    Cookies.set("session", "");
    var register_a = $(".register a");
    register_a.text("Register");
    register_a.attr("href", "#register_modal");
    var login_a = $(".login a");
    login_a.text("Login");
    login_a.attr("href", "#login_modal");
    login_a.attr("rel", "modal:open");
    register_a.attr("onclick", "");
    var create_a = $(".create");
    create_a.text("countr");
    create_a.attr("href", "");
    create_a.attr("rel", "");
    close_all_modals();
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
        if(typeof data == "string") {
            // data = JSON.parse(data);
            alert("An error occurred");
        } else if("error" in data) {
            data = JSON.parse(data);
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

function remove_counter(t) {
    event.preventDefault();
    var username = Cookies.get("username");
    var session = Cookies.get("session");
    var cid = $(t).data("value");

    // console.log(username +" "+ password);
    $.ajax({
        type: 'POST',
        url: "/remove_counter",
        data: JSON.stringify({"username":username, "counter_id":cid, "session_key": session})
    }).done(function(data) { 
        data = JSON.parse(data);
        if( "error" in data) {
            alert(data.error);
        } else {
            refresh();
        }
    });
}

function increment_counter(t) {
    t = $(t);
    var cid = t.data("value");
    var username = Cookies.get("username");
    $.ajax({
        type: 'POST',
        url: "/increment_count",
        data: JSON.stringify({"username":username, "counter_id":cid})
    }).done(function(data) { 
        data = JSON.parse(data);
        if("error" in data) {
            alert(data.error);
        } else {
            refresh();
        }
    });
}

function decrement_counter(t) {
    t = $(t);
    var cid = t.data("value");
    var username = Cookies.get("username");
    $.ajax({
        type: 'POST',
        url: "/decrement_count",
        data: JSON.stringify({"username":username, "counter_id":cid})
    }).done(function(data) {
        data = JSON.parse(data); 
        if("error" in data) {
            alert(data.error);
        } else {
            refresh();
        }
    });
}

function get_user_counter() {
    // event.preventDefault();
    var username = Cookies.get("username");
    if(!username) {
        alert("Sorry, I can't do that");
    } else {
    // console.log(username +" "+ password);
        $.ajax({
            type: 'POST',
            url: "/get_user_counters",
            data: JSON.stringify({"username":username}),

        }).done(function(data){
            console.log(data);
            // data = JSON.parse(data); 
            if (typeof data == "string") {
                logout();
                alert("An error occurred");
            } else {
                // console.log("here");
                var tuple = get_tuple(data);
                var keys = tuple[0];
                var vals = tuple[1];
                console.log(keys);
                console.log(vals);
                var grid = $(".pure-g");
                grid.empty();
                for (k in keys) {
                    k = keys[k];
                    var share_str = '<a target="_blank" href="/get_count?id='+k+'&username='+username+'">Share</a>';
                    var del_str = '<a onclick="remove_counter(this)" href="#" class="del" data-value="'+k+'">Delete</a>';
                    var inc = '<a onclick="increment_counter(this)" href="#" class="inc" data-value="'+k+'">Upvote</a>';
                    var dec = '<a onclick="decrement_counter(this)" href="#" class="dec" data-value="'+k+'">Downvote</a>';
                    var action_container = '<div class="action_container">'+inc+dec+'</div>';
                    var html_str = '<div class="pure-u-1-3 countr"><p data-value=\"'+k+'\">'+data[k]+'</p>'+share_str+action_container+del_str+'</div>';
                    grid.append(html_str);
                }
                close_all_modals();
            }
        });
    }
}

function check_if_we_logged_in_bro() {
    var c_key = Cookies.get("session");
    var u = Cookies.get("username");
    console.log(c_key);
    console.log(u);
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