This program is made by
2015113163_��ȿ��
2015117619 ������

# C program simulating mips datapath and control

> ������
- lmplementing Mode and Output(printing result into txt file)
- Implementing Signal of control unit and ALU control signal
- Making test cases for testing this program

> ��ȿ��
- Implementing basic structure of program
- Implementing functions and their connections
- Testing program


> init();
�� �Լ����� ��������, �ν�Ʈ���� & ������ �޸�, PC�� �ʱ�ȭ �ϰ� runme.hex�������̳ʸ��� �� ��ɾ �о��� �ν��ͷ¼� �޸𸮿� ������� �����Ѵ�.

> convert();
�� �Լ����� �������� ����� ���� �� ��ȣ�� � �������͸� �ǹ��ϴ��� ��Ʈ������ ��ȯ�Ѵ�.

> fetch();
�� �Լ����� ���� PC�� �ε����� ������ �������Ϳ� ����� ��ɾ �޾� �����Ѵ�. ���� jŸ�� ��ɾ �ɷ��� jŸ���̸� pc�� �����ϰ� ��ɾ� ������ ���� �ΰ����� ���� �����Ѵ�.

> decode();
�� �Լ��� fetch���� �޾ƿ� ��ɾ �޾� op �ʵ带 ���� � ��ɾ����� �Ǻ��ϰ� �׿� ���� ��Ʈ�� �ñ׳��� �Ҵ��Ͽ� �ش�. �� �� ��ɾ��� �� rs, tr, rd, smt, fnct(or const, adress)  �κ��� �а� ��Ʈ�� �ñ׳ο� ���߾� �´� ���� �������Ϳ� write�� ��ġ�� �����Ѵ�.

> exe();
�� �Լ����� ���ڵ� �ܰ迡�� ���� �ʵ带 �ε����� �ϴ� ���������� ������, ��ɾ� ���ڸ� 16��Ʈ�� �޾ƿ��� ��Ʈ�� �ñ׳ΰ� ����ʵ带 �˸°� �����Ͽ� ALU �ñ׳��� �����Ѵ�.
�� �� ��Ƽ�÷����� ������ �˸��� �����͵�� ALU �ñ׳ο� �´� ������ �����ϰ� ������� �����Ѵ�. ���� �� �����Ͱ� �����鼭 ��Ʈ�� �ñ׳� branch�� 1�̶�� PC�� �˸��� ������ �����Ѵ�.

> mem();
�� �Լ����� exe �ܰ迡�� ���� �޸��� �ּҿ� �޸𸮿� ���� �����͸� �޾ƿ���ñ׳ο� ���� �޸𸮿� ���� �����ϰų� �޸𸮿� �ִ� ���� �����Ѵ�.

> wb();
�� �Լ����� decode �ܰ迡�� �޾ƿ� write�� ��ġ�� alu���� ���Ǿ� ���� �����, ������ ������ ���� �޾ƿ� ��Ʈ�� �ñ׳� RegWrite�� 1�̶�� ��ġ�� ������ ���� �����Ѵ�.