#!/usr/bin/env python3
# vim: noexpandtab:ts=4:sts=4:sw=4

import pexpect
import subprocess
import os
import sys

DBNAME = 'test.db'
SQL_DUMP = 'sql1.is'

FNULL = open(os.devnull, 'w')

def remove(file):
	if os.path.isfile(file):
		os.remove(file)

def check_binaries():
	status, result = subprocess.getstatusoutput(ROSHI)
	if status != 0:
		print('\033[31mFAILURE\033[37m Make sure to have roshi compiled')
		sys.exit(1)

	binaries = ["diff", "sqlite3"]

	for b in binaries:
		status, result = subprocess.getstatusoutput(b + " --version")
		if status != 0:
			print(b)
			sys.exit(1)

def test_newlog():
	#ROSHI NEWLOG
	subprocess.run([ROSHI, DBNAME, 'newlog', '-e'], stdout=FNULL, stderr=subprocess.STDOUT)

	os.system("sqlite3 -line " + DBNAME + " 'select * from Exercises;' > " + SQL_DUMP)

	ret = os.system('diff expected_result_newlog_1 ' + SQL_DUMP)
	if ret == 0:
		print('newlog: \033[32mSUCCESS\033[37m')
	else:
		print('newlog: \033[31mFAILURE\033[37m')

def test_add():
	#ROSHI ADD
	child = pexpect.spawnu(ROSHI + ' ' + DBNAME + ' add')
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
		child.expect('Weight \(kg\): ')
		child.sendline('100')
		child.expect('Sets: ')
		child.sendline('5')
		child.expect('Reps: ')
		child.sendline('5')
		child.expect('Time/Duration \(min\): ')
		child.sendline('')
		child.expect('Tempo: ')
		child.sendline('4010')
		child.expect('Rest time \(min\): ')
		child.sendline('2')
		child.expect('Warmup set: ')
		child.sendline('')
		child.expect('Notes: ')
		child.sendline('Some notes')
		child.expect('Tags: ')
		child.sendline('tag1,tag2,tag3')
		child.expect('Exercise name: ')
		child.sendline('')

		os.system("sqlite3 -line " + DBNAME + " 'select * from Exercises;' > " + SQL_DUMP)
		ret = os.system('diff expected_result_add_1 ' + SQL_DUMP)
		if ret != 0:
			print('add: \033[31mFAILURE\033[37m')
		else:
			print('add: \033[32mSUCCESS\033[37m')
	except (pexpect.EOF,pexpect.TIMEOUT) as e:
		print('add: \033[31mFAILURE\033[37m')
		print(e)

	if child.isalive():
		child.close()

if __name__ == "__main__":
	# in case we run with CMake
	if len(sys.argv) > 2:
		ROSHI = sys.argv[2] + '/roshi'
	# in case we run manually
	else:
		ROSHI = '../roshi'

	check_binaries()

	if len(sys.argv) > 1:
		if sys.argv[1] == "newlog":
			print("Running test: newlog")
			test_newlog()
		elif sys.argv[1] == "add":
			print("Running test: add")
			subprocess.run([ROSHI, DBNAME, 'newlog', '-e'], stdout=FNULL, stderr=subprocess.STDOUT)
			test_add()
		else:
			print("No such test available")
			sys.exit(1)
	else:
		print("Running all tests..")
		test_newlog()
		test_add()

	remove(DBNAME)
	remove(SQL_DUMP)
