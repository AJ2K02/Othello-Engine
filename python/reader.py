import struct

WTHOR_PATH = '../wthor/'

class Game:
	def __init__(self, move_str, black_player, white_player, score, perfect_score):
		self.moves = move_str
		self.black_player = black_player
		self.white_player = white_player
		self.perfect_score = perfect_score
		self.score = score

	def to_string(self):
		string = str(self.black_player) + '\n' \
				+ str(self.white_player) + '\n' \
				+ str(self.score) + '\n' \
				+ str(self.perfect_score) + '\n' \
				+ self.moves + '\n' 
		return string

def load_players_list(filename):
	'''
	Returns a list of strings containing the (properly formatted)
	names of the players written in the file <filename>.
	'''
	file = open(filename, 'r')
	text = file.read()
	file.close()
	#format of lines : rank <space> name
	lines = text.split('\n')[:-1]
	#remove the ranks and keep the rest. Format : first name <space> last name
	names = [line[line.find(' ')+1:] for line in lines]
	ret = []
	for name in names:
		space = name.find(' ') 
		ret.append(name[space+1] + name[space+2:].lower() + ' ' + name[:space])
	return ret

def find_player_by_name(name):
	'''
	Finds the index of the given player in the WTHOR.JOU file.
	Parameters:
		name : string containing the name of the player to find
	Returns a number
	'''
	file = open(WTHOR_PATH+'WTHOR.JOU')
	inside = file.read()

	player_nb = -1
	for i in range(70000):
		if inside[i:i+20].startswith(name):
			player_nb = i
			break
	return (i-16)//20

def find_players_by_name(names):
	'''
	Similar to find_player_by_name, but for multiple players.
	Note : inefficient because it loops through the whole data
	for each player, but it doesn't matter.
	Parameters:
		names : list of strings containing names
	'''
	nbs = []
	for name in names:
		nbs.append(find_player_by_name(name))
	return nbs

def find_games_by_player(players_nb, colour='any'):
	'''
	Finds all the games played by the player with id player_nb.
	Parameters : 
		players_nb : IDs of the players we are intersted in 
		colour : string containing either 'black', 'white' or 'any'
			It allows to only pick games where the player plays the desired colour.
			Defaults to 'any'.
	Returns the binary data of all games of interest.
	Format : 
		short : black player index
		short : white player index
		byte : score
		byte : perfect_score
		byte[60] : moves
	'''

	# tuple to know if we are interested in games as black and as white
	colour = (1, 1) if colour == 'any' else (colour == 'black', colour == 'white')

	nb_games = 0
	data = bytes(0)
	for year in range(2000, 2019):
		#read all games
		games = open(WTHOR_PATH+f'WTH_{year}.wtb', 'rb')
		p = games.read()
		print('Year :', struct.unpack('h', p[10:12])[0], '- Depth :', p[14])
		for i in range(16, len(p), 68):
			#if we are interested in the game
			if (colour[0] and struct.unpack('h', p[i+2:i+4])[0] in players_nb) or (colour[1] and struct.unpack('h', p[i+4:i+6])[0] in players_nb):
				data += p[i+2:i+68]
				nb_games += 1
	print(f'{nb_games} games found.')
	return data

def bytes_to_list(data):
	gl = []
	for i in range(0, len(data), 66):
		game_str = []
		#add all moves to the list
		for m in range(60):
			game_str.append('abcdefgh'[data[i+6+m]%10-1] + str(data[i+6+m]//10))
		#create Game object
		game = Game(''.join(game_str),
					struct.unpack('h', data[i:i+2]), 
					struct.unpack('h', data[i+2:i+4]),
					(data[i+4]-32)*2,
					(data[i+5]-32)*2)
		gl.append(game)
	print(f'{len(gl)} games converted.')
	return gl


names = load_players_list('top200.txt')
print(names)

pnbs = find_players_by_name(names)
print('Players numbers :', pnbs)

games_data = find_games_by_player(pnbs)
games = bytes_to_list(games_data)

#save binary data
obf = open('GamesTop200.bin', 'wb')
obf.write(struct.pack('I', len(games_data)//66) + games_data)
obf.close()
'''
#save text data
otf = open('Games.txt', 'w')
for game in games:
	otf.write(game.to_string())
otf.close()
'''