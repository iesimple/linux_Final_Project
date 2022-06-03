# 输入文件自动生成
import random
import re


class FileGenerate():
    def __init__(self, filepath, room_n, customer_n) -> None:
        random.seed()
        self.ids = []
        self.all_requests = []
        self.id_threshold = 0.9
        self.filepath = filepath if filepath != '' else './in.txt'
        self.room_n = room_n
        self.customer_n = customer_n
        self.commandList = [
            'reserve', 'reserveblock', 'reserveany', 'cancel', 'cancelany',
            'cancelblock', 'check'
        ]

    def file_gen(self):
        """输入文件生成

        Args:
            room_n (int): 房间总数
            customer_n (int): 顾客总数
        """

        self._ids_gen(self.room_n)
        self._allRequest_gen(self.customer_n)

        self._writeTofile()

    def _ids_gen(self, room_n):
        """房间号生成，从1开始生成，设置出现房间号不连续的概率为0.15

        Args:
            room_n (int): 房间总数
        """
        cur_id = 1
        for i in range(room_n):
            if random.random() > self.id_threshold:
                cur_id += 1
            self.ids.append(cur_id)
            cur_id += 1

    def _allRequest_gen(self, customer_n):
        """所有用户所有请求的生成

        Args:
            customer_n (int): 顾客总数
        """
        for i in range(customer_n):
            name = 'customer'
            if random.random() > 0.9:
                name = name + str(random.randint(0, i))
            else:
                name = name + str(i)
            self.all_requests.append(
                [name, self._requestList_gen(name), 'end'])

    def _write_endl(self, fp, s):
        fp.write(s + '\n')

    def _writeTofile(self):
        """生成结果写入文件
        """
        with open(self.filepath, 'w') as fp:
            self._write_endl(fp, str(self.room_n))  # room_num
            for id in self.ids:  # room_ids
                self._write_endl(fp, str(id))
            self._write_endl(fp, str(self.customer_n))  # customer_n
            for customer_request in self.all_requests:  # each customer
                self._write_endl(fp, customer_request[0])  # customer name
                for request in customer_request[1]:  # requestList
                    self._write_endl(fp, ' '.join(str(i) for i in request))
                self._write_endl(fp, customer_request[-1])  # end

    def _requestList_gen(self, name):
        """一位顾客请求序列生成

        Args:
            name (string): 顾客姓名

        Returns:
            list: 每个元素都是个请求
        """
        requestList = []
        len = random.randint(1, 10)
        for i in range(len):
            request = self._request_gen(i)
            request.insert(-1, name)
            requestList.append(request)
        return requestList

    def _request_gen(self, i, rsvList=[]):
        """"""
        if i < 2:  # 保证前三个请求都是reserve类型的，主要是防止无意义的请求太多
            command = random.choice(self.commandList[0:3])
        else:
            command = random.choice(self.commandList)
        rsvList.append(self._parse_gen(command, rsvList))
        return rsvList[-1]

    # """
    # reserve 房间号 年 月 日 预约天数 预约姓名 操作时间
    # cancel 房间号 年 月 日 预约天数 预约姓名 操作时间
    # reserveblock 房间数 第一个房间号 年 月 日 预约天数 预约姓名 操作时间
    # cancelblock 房间数 第一个房间号 年 月 日 预约天数 预约姓名 操作时间
    # reserveany 房间数 年 月 日 预约天数 预约姓名 操作时间
    # cancelany 房间数 年 月 日 预约天数 预约姓名 操作时间
    # check 预约姓名 操作时间
    # """

    def _parse_gen(self, command, rsvList):
        """分析command类型生成符合规则的请求

        Args:
            command (string): 请求的第一个字符串
            rsvList (list): 暂存该顾客之前的请求

        Returns:
            list: 一条请求
        """
        days = [0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31]
        request = [
            command,
        ]
        if re.match(r'.*block', command, 0) or re.match(r'.*any', command, 0):
            request.append(
                random.randint(
                    1, min(self.room_n, min(10, int(self.room_n / 10)))))
        if command == 'reserve' or command == 'reserveblock':
            request.append(random.randint(1, max(self.ids)))
        if command != 'check':
            if re.match(r'cancel.*', command, 0):  # 对于cancel这一类的请求，年月日

                index = random.choice(range(len(rsvList)))
                while (rsvList[index][0] == 'check'):
                    index = random.choice(range(len(rsvList)))
                room_id = 1
                if re.match(r'.*block', rsvList[index][0], 0):
                    room_id = rsvList[index][2]
                elif re.match(r'.*any', rsvList[index][0], 0):
                    room_id = random.randint(1, max(self.ids))
                else:
                    room_id = rsvList[index][1]
                request.append(room_id)
                year = rsvList[index][-6]
                month = rsvList[index][-5]
                day = rsvList[index][-4]
                reserve_days = random.randint(1, rsvList[index][-3])
                request.extend([year, month, day, reserve_days])
            else:
                year = random.randint(0, 1) + 2022
                month = random.randint(1, 12)
                day = random.randint(1, days[month])
                reserve_days = random.randint(
                    1,
                    max(
                        1,
                        min(10, (2023 - year) * 365 + (12 - month) * 28 +
                            (days[month] - day))))
                request.extend([year, month, day, reserve_days])
        # sleep函数的输入要求是秒
        if self.customer_n < 10:
            request.append(random.randint(1, 5))  # 操作时间
        else:
            request.append(0)  # 在顾客数量多的时候，操作时间设置会影响测试时间
        return request


if __name__ == '__main__':
    filepath = input('生成文件输出路径，直接回车使用当前路径：')
    room_n = int(input('需要生成的房间总数：'))
    customer_n = int(input('顾客总数：'))
    generator = FileGenerate(filepath, room_n, customer_n)
    generator.file_gen()