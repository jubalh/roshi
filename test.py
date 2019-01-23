#!/usr/bin/env python3
# vim: noexpandtab:ts=4:sts=4:sw=4

import pexpect
import subprocess
import os
import sys

DBNAME = 'test.db'
SQL_DUMP = 'sql1.is'

def check_binaries():
	binaries = ["diff", "sqlite3"]
	for b in binaries:
		status, result = subprocess.getstatusoutput(b + " --version")
		if status != 0:
			print(b)
			sys.exit(1)

def main():
	#ROSHI NEWLOG
	try:
		subprocess.run(['./roshi', DBNAME, 'newlog', '-e'])
	except:
		print('\033[31mFAILURE\033[37m Make sure to have roshi compiled')
		sys.exit(1)

	os.system("sqlite3 -line " + DBNAME + " 'select * from Exercises;' > " + SQL_DUMP)

	ret = os.system('diff tests/expected_result_1 ' + SQL_DUMP)
	if ret == 0:
		print('Newlog: \033[32mSUCCESS\033[37m')
	else:
		print('Newlog: \033[31mFAILURE\033[37m')

	#ROSHI ADD
	child = pexpect.spawnu('./roshi ' + DBNAME + ' add')
	try:
		#Session
		child.expect('Session name: ')
		child.send('Weight\t\n')
		child.expect('Place: ')
		child.sendline('Leimen')
		child.expect('Date: ')
		child.send('\n')
		child.expect('Start time: ')
		child.send('\n')
		child.expect('End time: ')
		child.send('\n')
		child.expect('Notes: ')
		child.sendline('Best training ever')
		child.expect('Feeling \(-2 to 2\): ')
		child.send('\n')
		#Exercise
		child.expect('Exercise name: ')
		child.send('Sq\t\n')
		child.expect('Station: ')
		child.sendline('A1')
		child.expect('Weight: ')
		child.sendline('100')
		child.expect('Sets: ')
		child.sendline('5')
		child.expect('Reps: ')
		child.sendline('5')
		child.expect('Time/Duration: ')
		child.sendline('')
		child.expect('Tempo: ')
		child.sendline('4010')
		child.expect('Rest time: ')
		child.sendline('120')
		child.expect('Warmup set: ')
		child.sendline('')
		child.expect('Notes: ')
		child.sendline('Some notes')
		child.expect('Tags: ')
		child.sendline('tag1,tag2,tag3')
		child.expect('Exercise name: ')
		child.sendline('')
	except (EOF,TIMEOUT) as e:
		print(e)

	if child.isalive():
		child.close()

	#ROSHI ADD
	child = pexpect.spawnu('./roshi ' + DBNAME + ' add')
	try:
		#Session
		child.expect('Session name: ')
		child.send('Weight\t\n')
		child.expect('Place: ')
		child.sendline('Leimen')
		child.expect('Date: ')
		child.send('\n')
		child.expect('Start time: ')
		child.send('\n')
		child.expect('End time: ')
		child.send('\n')
		child.expect('Notes: ')
		child.sendline('Best training ever')
		child.expect('Feeling \(-2 to 2\): ')
		child.send('\n')
		#Exercise
		child.expect('Exercise name: ')
		child.send('Sq\t\n')
		child.expect('Station: ')
		child.sendline('A1')
		child.expect('Weight: ')
		child.sendline('100')
		child.expect('Sets: ')
		child.sendline('5')
		child.expect('Reps: ')
		child.sendline('5')
		child.expect('Time/Duration: ')
		child.sendline('')
		child.expect('Tempo: ')
		child.sendline('4010')
		child.expect('Rest time: ')
		child.sendline('120')
		child.expect('Warmup set: ')
		child.sendline('')
		child.expect('Notes: ')
		child.sendline('Some notes')
		child.expect('Tags: ')
		child.sendline('tag1,tag2,tag3')
		child.expect('Exercise name: ')
		child.sendline('')
		print('Add: \033[32mSUCCESS\033[37m')
	except (EOF,TIMEOUT) as e:
		print('Add: \033[31mFAILURE\033[37m')
		print(e)

	if child.isalive():
		child.close()

	os.remove(DBNAME)
	os.remove(SQL_DUMP)

if __name__ == "__main__":
	check_binaries()
	main()
