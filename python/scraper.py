from bs4 import BeautifulSoup
import requests as r

URL = 'https://www.worldothello.org/ratings'
NB_PLAYERS = 200

#get page and parse it using bs
page_response = r.get(URL)
page = BeautifulSoup(page_response.content, 'html.parser')
print('Page loaded / parsed')

#find all entries in the table of players
players = page.find_all('tr', {'isofficial':'true'})
print('Players found')

plist = []
for p in players[:NB_PLAYERS]:
	#player name / player rating
	pn = p.find_all('td', {'class':'ratinglist nameOfPlayer'})[0].get_text()
	pr = p.find_all('td', {'class':'ratinglist rating'})[0].get_text()
	print(pr, pn)
	plist.append((pr, pn))

# write result in a new file
filename = 'top' + str(NB_PLAYERS) + '.txt'
file = open(filename, 'w')
for pr, pn in plist:
	file.write(pr + ' ' + pn + '\n')
file.close()

