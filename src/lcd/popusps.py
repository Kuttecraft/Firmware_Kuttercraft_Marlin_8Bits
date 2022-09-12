from tkinter import *
from tkinter import messagebox as MessageBox


root = Tk()

def test():
    #MessageBox.showinfo("Hola!","Hola mundo")
    #MessageBox.showwarning("Alerta!","Error no se pudo actualizar")
    #MessageBox.showerror("Un error!","esto es un error")

    #resultado = MessageBox.askquestion("seguro","que quierer salis")
    #if resultado == "yes":
    #    root.destroy()
    #resultado = MessageBox.askokcancel("Un error!","esto es un error")

    #resultado = MessageBox.askyesno("Un error!","esto es un error")
    #if resultado:
    #    root.destroy()

    #resultado = MessageBox.askretrycancel("reintentar","no se puede conectar")
    #if resultado:
    #    root.destroy()

Button(root, text="Click", command=test).pack()


root.mainloop()
