# Top Level Makefile for schedule programs

all: schedule_obj newsmstr_obj render_obj

schedule_obj: 
	cd schedule; make schedule

newsmstr_obj:
	cd newsmstr; make newsmstr

render_obj:
	cd render; make all

full_backup:
	rm -f ../backups/*;\
	cd ../../; tar cvf schedule.tar schedule;\
	zip -r schedule schedule;\
	mv schedule.tar  schedule/backups;\
	mv schedule.zip schedule/backups;\
	cd schedule/backups; gzip -c schedule.tar > \
	schedule.tgz; rm -f schedule.tar
	
src_backup:
	rm -f ../backups/*;\
	cd ../../;tar  cv -f schedule.tar --exclude data schedule;\
	zip -r schedule schedule;\
	zip schedule -d schedule/data/\*;\
	mv schedule.tar  schedule/backups;\
	mv schedule.zip schedule/backups;\
	cd schedule/backups; gzip -c schedule.tar > \
	schedule.tgz; rm -f schedule.tar

full_backup_next:
	rm -f ../backups/*;\
	cd ../../; tar cvf schedule.tar schedule;\
	mv schedule.tar  schedule/backups;\
	cd schedule/backups; gzip -c schedule.tar > \
	schedule.tgz; rm -f schedule.tar
	
src_backup_next:
	rm -f ../backups/*;\
	cd ../../;tar  cv -f schedule.tar --exclude data schedule;\
	mv schedule.tar  schedule/backups;\
	cd schedule/backups; gzip -c schedule.tar > \
	schedule.tgz; rm -f schedule.tar


clean:
	cd schedule; make clean; 
	cd newsmstr; make clean;
	cd render; make clean;

