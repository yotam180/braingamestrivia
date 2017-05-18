categories = {
    0: "Any category",
    9: "General Knowledge",
    10: "Entertainment: Books",
    11: "Entertainment: Film",
    12: "Entertainment: Music",
    13: "Entertainment: Musicals & Theatres",
    14: "Entertainment: Television",
    15: "Entertainment: Video Games",
    16: "Entertainment: Board Games",
    17: "Science & Nature",
    18: "Science: Computers",
    19: "Science: Mathematics",
    20: "Mythology",
    21: "Sports",
    22: "Geography",
    23: "History",
    24: "Politics",
    25: "Art",
    26: "Celebrities",
    27: "Animals",
    28: "Vehicles",
    29: "Entertainment: Comics",
    30: "Science: Gadgets",
    31: "Entertainment: Japanese Anime & Manga",
    32: "Entertainment: Cartoon & Animations",
}
numbers = {v:k for (k, v) in categories.iteritems()}
    

import urllib2
import json
import sqlite3

f = open("file.txt", "w")
res = json.loads(urllib2.urlopen("https://opentdb.com/api_token.php?command=request").read())
token = res["token"]
print token
res = json.loads(urllib2.urlopen("https://opentdb.com/api.php?amount=50&type=multiple&token=" + token).read())
while res["response_code"] == 0:
    for q in res["results"]:
        sql = "insert into Questions (Question, Correct, Ans1, Ans2, Ans3, Category, Difficulty) values ('%s', '%s', '%s', '%s', '%s', '%s', '%s');" % (q["question"].replace("'", "\\'"), q["correct_answer"].replace("'", "\\'"), q["incorrect_answers"][0].replace("'", "\\'"), q["incorrect_answers"][1].replace("'", "\\'"), q["incorrect_answers"][2].replace("'", "\\'"), numbers[q["category"]], q["difficulty"])
        f.write(sql.encode('utf8'))
        f.write("\n")
    res = json.loads(urllib2.urlopen("https://opentdb.com/api.php?amount=50&type=multiple&token=" + token).read())

f.close()