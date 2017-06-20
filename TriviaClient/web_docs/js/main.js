LoadFiles();

$(document).ready(function() {
    var score = 0;
    window.game = {
        players: 0,
        rounds: 0,
        round_time: 0,
        category: 0,
        difficulty: ""
    };
    var scanner = new Scanner("<@echo(_ip);@>", 54453).loading($("#loadingDiv")).ready($("#loginDiv"))
    .handler(106, function() {
        scanner.expect(1, function(data) {
            if (!data[0]) {
                 $("#roomlistDiv").html("");
                return;
            }
            var PACKET_SIZE = 10;
            scanner.expect(data[0] * PACKET_SIZE, function(rooms) {
                var tbl = "<table style='width: 100%; table-layout: fixed;' border='0'>";
                for (var i = 0; i < rooms.length; i += PACKET_SIZE) {
                    tbl += "<tr roomid='" + rooms[i] + "' style='margin-bottom: 10px; border: 1px solid gray;' class='"
                        + (rooms[i + 6] < rooms[i + 5] && rooms[4] == 0 ? "enabled_room" : "disabled_room")
                        + " roomlist_tr'><td style='width: 7%'>"
                        + rooms[i]
                        + "</td><td style='width: 7%'>"
                        + (rooms[i + 2] != '0' ? "<img src='asset/subject_" + rooms[i + 2] + ".png' />" : "&nbsp;")
                        + "</td><td style='width: 20%;'><b>"
                        + rooms[i + 1]
                        + "</b></td><td style='width: 10%; color: " + difficultyColors[rooms[i + 3]] + "'>"
                        + rooms[i + 3]
                        + "</td><td style='width: 20%;'>"
                        + categories[rooms[i + 2]]
                        + "</td><td style='width: 15%; text-align: center;'>"
                        + rooms[i + 6] + "/" + rooms[i + 5]
                        + (isMobile() ? "" : ("</td><td style='width: 15%; text-align: center;'>"
                        + rooms[i + 8] + " rounds<br/>" + rooms[i + 9] + " sec/round"))
                        + "</td></tr>";
                }
                tbl += "</table>";
                $("#roomlistDiv").html(tbl);
            });
        });
    })
    .handler(107, function() {
        scanner.expect(1, function(data) {
            if (!data[0]) return;
            scanner.expect(data[0], function(users) {
                var usrs = "<h2>Online:</h2><ul>";
                for (var i = 0; i < users.length; i++) {
                    usrs += "<li><b>" + users[i] + "</b></li>";
                }
                usrs += "</ul>";
                $("#playerlistDiv").html(usrs);
            });
        });
    })
    .handler(108, function() {
        scanner.expect(8, function(data) {
            if (data[7] == 1) {
                // Admin
                $("#settingSetDiv").css({backgroundColor: "whitesmoke"});
                $("#settingSetDiv").children().css({backgroundColor: ""});
                $("#settingSetDiv").children().not("select").prop('disabled',false);
                $("#btnCloseRoom").prop('disabled',false);
            }
            else {
                // Non-Admin
                $("#settingSetDiv").css({backgroundColor: "gray"});
                $("#settingSetDiv").children().css({backgroundColor: "gray"});
                $("#settingSetDiv").children().not("select").prop('disabled',true);
                $("#btnCloseRoom").prop('disabled',true);
            }
            $("#numPlayersChange").val(data[3]);
            game.players = data[3];
            $("#numRoundsChange").val(data[4]);
            game.rounds = data[4];
            $("#numRoundTimeChange").val(data[5]);
            game.round_time = data[5];
            $("#selDifficultyChange").val(data[2]);//.text(data[2]);
            game.difficulty = data[2];
            $("#selCategoryChange").val(data[1]);//.text(categories[data[1]]);
            game.category = data[1];
            scanner.expect(data[6], function(users) {
                var u = "<table style='width: 100%;' border='1'>";
                for (var x = 0; x < users.length; x++) {
                    u += "<tr><td style='text-align: center; margin: 10px; padding: 10px;'>" + users[x] + "</td></tr>";
                }
                u += "</table>";
                $("#roomplayersDiv").html(u);
            });
        });
    })
    .handler(116, function() {
        $("#mainDiv").show();
        $("#roomDiv").hide();
        alertify.alert("Room closed", "The room was closed by the administrator.");
    })
    .handler(1170, function() {
        $("#roomDiv").fadeOut();
        $("#gameDiv").fadeIn();
        $("#questionDiv").hide();
        $("#answersDiv").hide();
        score = 0;
    })
    .handler(118, function() {
        $("#questionDiv").show();
        $("#answersDiv").hide();
        $("#pbTime").css({width: "100%"});
        $("#pbTime").animate({width: "0%"}, game.round_time * 1000);
        scanner.expect(5, function(data) {
            console.log(data);
            $(".btnAnswer[answer='1']").css({backgroundColor: "white"}).text(data[1].toString().htmlDecode());
            $(".btnAnswer[answer='2']").css({backgroundColor: "white"}).text(data[2].toString().htmlDecode());
            $(".btnAnswer[answer='3']").css({backgroundColor: "white"}).text(data[3].toString().htmlDecode());
            $(".btnAnswer[answer='4']").css({backgroundColor: "white"}).text(data[4].toString().htmlDecode());
            $("#lblQuestion").text(data[0].toString().htmlDecode());
        });
    })
    .handler(130, function() {
        scanner.expect(4, function(d) {
            $(".btnAnswer[answer='" + d[0] + "']").css({backgroundColor: "green"});
            scanner.expect(d[3] * 2, function(u) {
                setTimeout(function() {
                    $("#questionDiv").hide();
                    $("#answersDiv").fadeIn();
                    $("#spnPlace").text(d[2]);
                    $("#spnPts").text(d[1]);
                    var tbl = "<table style='table-layout: fixed; width: 100%; font-size: larger;' border='2'>";
                    var p = [];
                    for (var i = 0; i < u.length; i+=2) {
                        p.push({"name": u[i], "score": u[i + 1]});
                    }
                    p.sort((a, b) => b.score - a.score);
                    for (var i = 0; i < p.length; i++) {
                        tbl += "<tr><td style='text-align: center;'>%s</td><td style='text-align: center;'>%s</td></tr>"
                                .replace("%s", p[i].name).replace("%s", p[i].score);
                    }
                    tbl += "</table>";
                    $("#divList").html(tbl);
                }, 1000);
            });
        });
    })
    .handler(121, function() {
        scanner.expect(3, function(d) {
            scanner.expect(d[2] * 2, function(u) {
                setTimeout(function() {
                    $("#questionDiv").hide();
                    $("#answersDiv").fadeIn();
                    $("#spnPlace").text(d[1]);
                    $("#spnPts").text(d[0]);
                    var tbl = "<table style='table-layout: fixed; width: 100%; font-size: larger;' border='2'>";
                    var p = [];
                    for (var i = 0; i < u.length; i+=2) {
                        p.push({"name": u[i], "score": u[i + 1]});
                    }
                    p.sort((a, b) => b.score - a.score);
                    for (var i = 0; i < p.length; i++) {
                        tbl += "<tr><td style='text-align: center;'>%s</td><td style='text-align: center;'>%s</td></tr>"
                                .replace("%s", p[i].name).replace("%s", p[i].score);
                    }
                    tbl += "</table>";
                    $("#divList").html(tbl);
                    $("#gameDiv").fireworks();
                    alertify.alert("You're #" + d[1] + "!", "You finished the game with a score of " + d[0] + "!");
                    setTimeout(function() {
                        $("#gameDiv").fireworks("destroy");
                        $("#gameDiv").hide();
                        $("#roomDiv").show();
                    }, 10000);
                }, 1000);
            });
        });
    })
    .handler(1301, function() {
        alertify.dismissAll();
        scanner.expect(2, function(data) {
            alertify.notify(data[0] + "/" + data[1] + " answered!", "success", 1);
        });
    });

    window.scanner = scanner;
    var session = {};

    if (isMobile()) {
        // Updating elements sizes to fit mobile phones
        $("#roomlistDiv").css({width: "100%", height: "80%"});
        $("#playerlistDiv").css({width: "0", height: "0", display: "none"});
        $(".bottomButton").css({height: "20%"});
        $("#chatDiv").css({width: 0});
        $("#roomplayersDiv").css({width: "30%", left: "0", height: "70%"});
        $("#settingSetDiv").css({width: "70%", height: "70%"});
        $(".ansButton").css({width: "90%", height: "15%"});
    }
    else {
        $(".ansButton").after("<br/>");
    }
    var opts = "";
    for (var x in categories) if (categories.hasOwnProperty(x)) {
        opts += "<option value='" + x + "'>" + categories[x] + "</option>";
    }
    $("#selCategory").html(opts);
    $("#selCategoryChange").html(opts);
    $("#lobbyMusic").animate({volume: 0}, 0);
    setInterval(tick, 100);
    var lstmsc = false;
    function tick() {
        if ($("#roomDiv").is(":visible") != lstmsc) {
            $("#lobbyMusic").animate({volume: $("#roomDiv").is(":visible")}, 1000)
            lstmsc = $("#roomDiv").is(":visible");
        }
    }

    $("#btnLogin").click(function() {

        scanner.send(200).send($("#txtUsername").val() || "yotam180").send($("#txtPassword").val() || "yotamsalmon")
        .callback(1020, function() {
            session["username"] = $("#txtUsername").val() || "yotam180";
            session["password"] = $("#txtPassword").val() || "yotamsalmon";
            $("#txtUsername").val("");
            $("#txtPassword").val("");

            $("#loginDiv").fadeOut();
            $("#mainDiv").fadeIn();
        })
        .callback(1021, function() {
            $("#lblError").html("Wrong credentials");
        })
        .callback(1022, function() {
            $("#lblError").html("User already connected");
        });
    });

    $("#btnSignupLink").click(function() {
        $("#loginDiv").fadeOut();
        $("#signupDiv").fadeIn();
    });

    $("#btnLoginLink").click(function() {
        $("#loginDiv").fadeIn();
        $("#signupDiv").fadeOut();
    });

    $("#btnCreateRoomLink").click(function() {
        $("#mainDiv").fadeOut();
        $("#createroomDiv").fadeIn();
    });

    $("#btnCancalRoomCreation").click(function() {
        $("#createroomDiv").fadeOut();
        $("#mainDiv").fadeIn();
    });

    $("#btnSignup").click(function() {
        var username = $("#txtUsernameSignup").val();
        var password = $("#txtPasswordSignup").val();
        var password_confirm = $("#txtPasswordConfirmSignup").val();
        var email = $("#txtEmailSignup").val();
        if (~username.indexOf("##")) {
            $("#lblErrorSignup").html("Username cannot contain '##'");
        }
        else if (~password.indexOf("##")) {
            $("#lblErrorSignup").html("Password cannot contain '##'");
        }
        else if (username == "") {
            $("#lblErrorSignup").html("Username cannot be empty");
        }
        else if (password == "") {
            $("#lblErrorSignup").html("Password cannot be empty");
        }
        else if (password != password_confirm) {
            $("#lblErrorSignup").html("Password does not match the confirmation");
        }
        else {
            scanner.send(203).send(username).send(password).send(email)
            .callback(1040, function() {
                $("#lblErrorSignup").html("You were signed up successfully! Redirecting...");
                setTimeout(function() {
                    $("#loginDiv").fadeIn();
                    $("#signupDiv").fadeOut();
                }, 3000);
            })
            .callback(1041, function() {
                $("#lblErrorSignup").html("Password is in illegal format.<br/>Can contain only A-Z a-z 0-9, underscore &amp; dot.");
            })
            .callback(1042, function() {
                $("#lblErrorSignup").html("The username is already taken.");
            })
            .callback(1043, function() {
                $("#lblErrorSignup").html("Username is in illegal format.<br/>Can contain only A-Z a-z 0-9, underscore &amp; dot.");
            })
            .callback(1044, function() {
                $("#lblErrorSignup").html("An unknown error occured.");
            });
        }
    });

    $("#btnCreateRoom").click(function() {
        var roomname = $("#txtRoomName").val() || "Dare to Think!";
        var numPlayers = parseInt($("#numPlayers").val() || "4");
        var numRounds = parseInt($("#numRounds").val() || "6");
        var roundTime = parseInt($("#numRoundTime").val() || "10");
        var category = $("#selCategory").val();
        var difficulty = $("#selDifficulty").val();
        if (~roomname.indexOf("##")) {
            $("#lblErrorCreation").html("Room name cannot contain '##'");
        }
        else if (category > 27 || category < 9) {
            category = 0;
        }
        if (!~["hard", "easy", "medium"].indexOf(difficulty)) {
            difficulty = "easy";
        }
        if (numPlayers < 2 || numPlayers > 10) {
            $("#lblErrorCreation").html("Max. Players must be between 2 and 10");
        }
        else if (numRounds < 1 || numRounds > 20) {
            $("#lblErrorCreation").html("Rounds number must be between 1 and 20");
        }
        else if (roundTime < 5 || roundTime > 60) {
            $("#lblErrorCreation").html("Goddam, please give a reasonable round time");
        }
        else {
            scanner.send(213).send(roomname).send(numPlayers).send(numRounds).send(roundTime).send(category).send(difficulty)
            .callback(1141, function() {
                $("#lblErrorCreation").html("The server has failed to create your room");
            })
            .callback(1140, function() {
                scanner.expect(1, function(data) {
                    var roomId = data[0];
                    session["room"] = roomId;
                    $("#createroomDiv").fadeOut();
                    $("#roomDiv").fadeIn();
                });
            });
        }
    });

    $("#btnLeaveRoomLink").click(function() {
        scanner.send(211).callback(112, function() {
            $("#roomDiv").fadeOut();
            $("#mainDiv").fadeIn();
        });
    });

    $(document).on('click', '.roomlist_tr', function() {
        let room = $(this).attr("roomid");
        scanner.send(209).send(room)
        .callback(1100, function() {
            session["room"] = room;
            $("#mainDiv").fadeOut();
            $("#roomDiv").fadeIn();
        })
        .callback(1101, function() {
            alertify.alert("Error", "This room is full");
        })
        .callback(1102, function() {
            alertify.alert("Error", "An error occured when trying to enter the game room");
        });
    });

    $(".autoupdate").change(function() {
        var numPlayers = parseInt($("#numPlayersChange").val() || "4");
        var numRounds = parseInt($("#numRoundsChange").val() || "6");
        var roundTime = parseInt($("#numRoundTimeChange").val() || "10");
        var category = $("#selCategoryChange").val();
        var difficulty = $("#selDifficultyChange").val();
        scanner.send(214).send(numPlayers).send(numRounds).send(roundTime).send(category).send(difficulty);
    });

    $("#btnCloseRoom").click(function() {
        scanner.send(215);
    });

    $("#btnStartGame").click(function() {
        scanner.send(217);
    });

    $(".btnAnswer").click(function() {
        let dis = $(this);
        scanner.send(219).send($(this).attr("answer"))
        .callback(1200, function() {
            dis.css({backgroundColor: "green"});
            $("#soundCorrect").get(0).play();
            scanner.expect(1, function(d) {
                score = d[0];
            });
        })
        .callback(1201, function() {
            dis.css({backgroundColor: "red"});
            $("#soundWrong").get(0).play();
        });
    });
});

String.prototype.htmlDecode = function () {
  return this.replace(/&amp;/g, '&')
            .replace(/&quot;/g, '"')
            .replace(/\&([0-9]+?)\;/g, function(x) { return String.fromCharCode(parseInt(x.substring(1, 3))); });
}

function htmlEncode(value) {
  return $('<textarea/>').text(value).html();
}

var categories = {
    0: "Any category",
    9: "General Knowledge",
    10: "Books",
    11: "Film",
    12: "Music",
    13: "Musicals & Theatres",
    14: "Television",
    15: "Video Games",
    16: "Board Games",
    17: "Science & Nature",
    18: "Computers",
    19: "Mathematics",
    20: "Mythology",
    21: "Sports",
    22: "Geography",
    23: "History",
    24: "Politics",
    25: "Art",
    26: "Celebrities",
    27: "Animals"
}

var difficultyColors = {
    "Easy": "green",
    "Medium": "orange",
    "Hard": "red"
}