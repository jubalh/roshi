#!/usr/bin/env python3
# vim: noexpandtab:ts=4:sts=4:sw=4
#
# Copyright (C) 2019 Michael Vetter <jubalh@iodoru.org>
# SPDX-License-Identifier: GPL-3.0-or-later

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

	os.system("sqlite3 -line " + DBNAME + " 'select * from Exercises, Sessions, Tags, ExercisesTags;' > " + SQL_DUMP)

	os.system("sed -i 's/Start =.*/Starta/g' " + SQL_DUMP)
	os.system("sed -i 's/End =.*/Enda/g' " + SQL_DUMP)
	ret = os.system('diff expected_result_newlog_1 ' + SQL_DUMP)
	if ret == 0:
		print('newlog: \033[32mSUCCESS\033[37m')
	else:
		print('newlog: \033[31mFAILURE\033[37m')
		sys.exit(1)

def test_add():
	#ROSHI ADD
	child = pexpect.spawnu(ROSHI + ' ' + DBNAME + ' add')
	try:
		#Session
		child.expect('Session name: ')
		child.sendline('Wei;ght')
		child.expect('Invalid input')
		child.expect('Session name: ')
		child.send('Weight\t\n')
		child.expect('Place: ')
		child.sendline('Leimen')
		# test invalid date; longer
		child.expect('Date: ')
		child.send('a\n')
		child.expect('Invalid input')
		# test invalid date; nr where digit
		child.expect('Date: ')
		child.send('\ba\n')
		child.expect('Invalid input')
		# test invalid date; wrong seperator
		child.expect('Date: ')
		child.send('\b\b\b111\n')
		child.expect('Invalid input')
		child.expect('Date: ')
		child.send('\n')
		# test invalid time
		child.expect('Start time: ')
		child.send('1\n')
		child.expect('Invalid input')
		child.expect('Start time: ')
		child.send('\ba\n')
		child.expect('Invalid input')
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
		child.expect('Distance \(m\): ')
		child.sendline('500')
		child.expect('Time/Duration \(min\): ')
		child.sendline('')
		child.expect('Tempo: ')
		child.sendline('40101')
		child.expect('Invalid input')
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
		child.send('D\t\n')
		child.expect('Station: ')
		child.sendline('A2')
		child.expect('Weight \(kg\): ')
		child.sendline('50')
		child.expect('Sets: ')
		child.send('\n')
		child.expect('Reps: ')
		child.send('\n')
		child.expect('Distance \(m\): ')
		child.sendline('')
		# test if minutes are transformed into seconds
		child.expect('Time/Duration \(min\): ')
		child.sendline('1')
		child.expect('Tempo: ')
		child.send('\n')
		child.expect('Rest time \(min\): ')
		child.sendline('5')
		# test if warmup set is correctly transformed
		child.expect('Warmup set: ')
		child.sendline('\b\byes')
		child.expect('Notes: ')
		child.sendline('Some more notes')
		child.expect('Tags: ')
		# test that we dont add the same tag twice
		child.sendline('tag1,tag4,tag5,tag5,tag6')
		child.expect('Exercise name: ')
		child.sendline('')

		os.system("sqlite3 -line " + DBNAME + " 'select * from Exercises, Tags;' > " + SQL_DUMP)
		ret = os.system('diff expected_result_add_1 ' + SQL_DUMP)
		if ret != 0:
			print('add: \033[31mFAILURE\033[37m')
			sys.exit(1)
		else:
			print('add: \033[32mSUCCESS\033[37m')
	except (pexpect.EOF,pexpect.TIMEOUT) as e:
		print('add: \033[31mFAILURE\033[37m')
		print('before: ' + child.before)
		print('after: ' + child.after)
		print(e)
		sys.exit(1)

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
	sys.exit(0)
