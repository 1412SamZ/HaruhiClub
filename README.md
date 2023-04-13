# HaruhiClub

## deps
sudo apt-get install mysql-server
libmysql++-dev


## TODOs

### 获取服务器进程信息
### 可以杀死进程

### 查询服务器cpu 内存 网络io 磁盘io
### 警告系统 通过邮件发送警告

### 定时执行shell脚本

### 将用户记录写入日志 存储到数据库
### 配置相应的日志查询功能

## 更新日志

### 3.29
#### 创建git repo
#### 继承旧任务 
#### 写好了sql封装代码（未上传）， socket封装
#### 开始写系统调用函数封装



### 3.30
#### 写线程池，多路io复用

### 3.31
#### 3.30没干完的接着干
#### 确立服务器主程序结构 从简单的开始



### 4.3
进程管理器
进程pid cpu占用 内存占用 磁盘网络io

pid： proc下数字命名的文件夹
cpu占用：proc/pid/stat中更新记录的信息，其中14 15列记载utime和stime，delta（u+s）是一段时间内的进程占用时间
单位是clock tick，除以这段时间换算成的clock tick
sysconf(_SC_CLK_TCK)返回系统一秒有多少个clock tick，总公式为delta（utime + stime）/（clocktick*time）

Mem: 从proc下获取

IO：从pidstat -d读取，io文件需要root权限 不爽
PID：进程id
kB_rd/s：每秒从磁盘读取的KB
kB_wr/s：每秒写入磁盘KB
kB_ccwr/s：任务取消的写入磁盘的KB。当任务截断脏的pagecache的时候会发生。
COMMAND:task的命令名

cpu和内存也可以从ps aux读取 更方便

网络 proc/pid/net/dev下有bytes字段 隔时间计算就好


### 4.12
mysql 创建表语句

CREATE TABLE IF NOT EXISTS login(
   id INT UNSIGNED AUTO_INCREMENT,
   username VARCHAR(100) NOT NULL,
   password VARCHAR(40) NOT NULL,
   PRIMARY KEY ( id )
)default charset = utf8;

添加用户
insert into login(username, password) values('jack', 'jack');