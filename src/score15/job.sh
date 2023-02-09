#!/bin/sh

for name in `cat foo`
do
	score -r "$name" >> bar
done
